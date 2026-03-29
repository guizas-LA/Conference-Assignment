#include "../header/assignment.h"
#include "../header/maxflow.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

string resolveDatasetPath(const string &fileName, const string &folderName) {
    fs::path path(fileName);
    if (path.is_absolute()) {
        return path.string();
    }

    fs::path current = fs::current_path();
    fs::path currentDataset = current / "dataset";
    fs::path parentDataset = current.parent_path() / "dataset";

    if (fs::exists(currentDataset / folderName)) {
        return (currentDataset / folderName / path).string();
    }

    if (fs::exists(parentDataset / folderName)) {
        return (parentDataset / folderName / path).string();
    }

    if (fs::exists(currentDataset)) {
        return (currentDataset / path).string();
    }

    if (fs::exists(parentDataset)) {
        return (parentDataset / path).string();
    }

    return (current / path).string();
}

static int solvePrimaryFlow(const vector<Submission> &subs,const vector<Reviewer> &revs,
                            int minReviews,int maxReviews,int excludedReviewerIndex,
                            vector<Assignment> *assignments,vector<MissingReviews> *missing) {
    int submissionCount = subs.size();
    int reviewerCount = revs.size();

    int source = submissionCount + reviewerCount;
    int sink = submissionCount + reviewerCount + 1;
    int nodeCount = submissionCount + reviewerCount + 2;

    vector<vector<int>> capacity(nodeCount, vector<int>(nodeCount, 0));
    vector<vector<int>> adj(nodeCount);

    auto addEdge = [&](int u, int v, int cap) {
        capacity[u][v] = cap;
        adj[u].push_back(v);
        adj[v].push_back(u);
    };

    for (int i = 0; i < submissionCount; i++) {
        addEdge(source, i, minReviews);
    }

    for (int i = 0; i < submissionCount; i++) {
        for (int j = 0; j < reviewerCount; j++) {
            if (j == excludedReviewerIndex) continue;
            if (subs[i].primary == revs[j].primary) {
                addEdge(i, submissionCount + j, 1);
            }
        }
    }

    for (int j = 0; j < reviewerCount; j++) {
        if (j == excludedReviewerIndex) continue;
        addEdge(submissionCount + j, sink, maxReviews);
    }

    int totalFlow = edmondsKarp(capacity, adj, source, sink);

    if (assignments != nullptr) assignments->clear();
    if (missing != nullptr) missing->clear();

    for (int i = 0; i < submissionCount; i++) {
        int assignedToSubmission = 0;
        for (int j = 0; j < reviewerCount; j++) {
            if (j == excludedReviewerIndex) continue;
            if (capacity[i][submissionCount + j] > 0) {
                assignedToSubmission += capacity[i][submissionCount + j];
                if (assignments != nullptr) {
                    assignments->push_back({subs[i].id, revs[j].id, subs[i].primary});
                }
            }
        }

        if (missing != nullptr && assignedToSubmission < minReviews) {
            missing->push_back({subs[i].id, subs[i].primary, minReviews - assignedToSubmission});
        }
    }

    return totalFlow;
}

void primaryAssignments(vector<Submission> &subs,vector<Reviewer> &revs,const map<string,string> &params,
                        vector<Assignment> &assignments,vector<MissingReviews> &missing) {
    assignments.clear();
    missing.clear();

    int minReviews = stoi(params.at("MinReviewsPerSubmission"));
    int maxReviews = stoi(params.at("MaxReviewsPerReviewer"));
    solvePrimaryFlow(subs, revs, minReviews, maxReviews, -1, &assignments, &missing);

    cout << "Assignments generated using MaxFlow.\n";
    if (!missing.empty()) {
        cout << "Some submissions could not receive the requested minimum number of reviews.\n";
    }
}

void writeAssignments(const vector<Assignment> &assignments,const string &outputFileName,
                      const vector<MissingReviews> &missing) {
    fs::path resolvedOutputPath(resolveDatasetPath(outputFileName, "output"));
    ofstream out(resolvedOutputPath);

    if (!out.is_open()) {
        cerr << "Error opening output file: " << resolvedOutputPath.string() << "\n";
        return;
    }

    writeAssignments(out, assignments, missing);
    out.close();

    cout << "Assignments written to " << resolvedOutputPath.string() << endl;
}

void writeAssignments(ostream &out,const vector<Assignment> &assignments,
                      const vector<MissingReviews> &missing) {
    out << "#SubmissionId,ReviewerId,Match\n";

    for (const auto &a : assignments) {
        out << a.submissionId << ", " << a.reviewerId << ", " << a.match << "\n";
    }

    out << "#ReviewerId,SubmissionId,Match\n";

    for (const auto &a : assignments) {
        out << a.reviewerId << ", " << a.submissionId << ", " << a.match << "\n";
    }

    out << "#Total: " << assignments.size() << "\n";

    if (!missing.empty()) {
        out << "#SubmissionId,Domain,MissingReviews\n";

        for (const auto &entry : missing) {
            out << entry.submissionId << ", " << entry.domain << ", " << entry.missing << "\n";
        }
    }
}

bool analyzeReviewerRisk(const vector<Submission> &subs,const vector<Reviewer> &revs,
                         const map<string,string> &params,vector<RiskResult> &criticalReviewers) {
    criticalReviewers.clear();

    int minReviews = stoi(params.at("MinReviewsPerSubmission"));
    int maxReviews = stoi(params.at("MaxReviewsPerReviewer"));
    int requiredFlow = static_cast<int>(subs.size()) * minReviews;

    vector<Assignment> baselineAssignments;
    vector<MissingReviews> baselineMissing;
    int baselineFlow = solvePrimaryFlow(subs, revs, minReviews, maxReviews, -1,
                                        &baselineAssignments, &baselineMissing);

    if (baselineFlow < requiredFlow) {
        return false;
    }

    for (int reviewerIndex = 0; reviewerIndex < static_cast<int>(revs.size()); reviewerIndex++) {
        vector<MissingReviews> missing;
        int flowWithoutReviewer = solvePrimaryFlow(subs, revs, minReviews, maxReviews, reviewerIndex,
                                                   nullptr, &missing);

        if (flowWithoutReviewer < requiredFlow) {
            criticalReviewers.push_back({revs[reviewerIndex].id, missing});
        }
    }

    sort(criticalReviewers.begin(), criticalReviewers.end(),
         [](const RiskResult &lhs, const RiskResult &rhs) {
             return lhs.reviewerId < rhs.reviewerId;
         });

    return true;
}

void writeRiskAnalysis(const vector<RiskResult> &criticalReviewers,const string &outputFileName) {
    fs::path resolvedOutputPath(resolveDatasetPath(outputFileName, "output"));
    ofstream out(resolvedOutputPath);

    if (!out.is_open()) {
        cerr << "Error opening risk output file: " << resolvedOutputPath.string() << "\n";
        return;
    }

    writeRiskAnalysis(out, criticalReviewers);
    out.close();
    cout << "Risk analysis written to " << resolvedOutputPath.string() << endl;
}

void writeRiskAnalysis(ostream &out,const vector<RiskResult> &criticalReviewers) {
    out << "#Risk Analysis: 1\n";

    if (criticalReviewers.empty()) {
        out << "\n";
    }
    else {
        for (size_t i = 0; i < criticalReviewers.size(); i++) {
            if (i > 0) out << ", ";
            out << criticalReviewers[i].reviewerId;
        }
        out << "\n";
    }
}
