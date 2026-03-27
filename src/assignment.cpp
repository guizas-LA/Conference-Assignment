#include "../header/assignment.h"
#include "../header/maxflow.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

static fs::path resolveOutputPath(const string &outputFileName) {
    fs::path outputPath(outputFileName);
    if (outputPath.is_absolute()) {
        return outputPath;
    }

    fs::path current = fs::current_path();
    if (fs::exists(current / "data")) {
        return current / outputPath;
    }

    if (fs::exists(current.parent_path() / "data")) {
        return current.parent_path() / outputPath;
    }

    return current / outputPath;
}

static int solvePrimaryFlow(const vector<Submission> &subs,const vector<Reviewer> &revs,
                            int minReviews,int maxReviews,int excludedReviewerIndex,
                            vector<Assignment> *assignments,vector<MissingReviews> *missing) {
    int S = subs.size();
    int R = revs.size();

    int source = S + R;
    int sink = S + R + 1;
    int V = S + R + 2;

    vector<vector<int>> capacity(V, vector<int>(V, 0));
    vector<vector<int>> adj(V);

    auto addEdge = [&](int u, int v, int cap) {
        capacity[u][v] = cap;
        adj[u].push_back(v);
        adj[v].push_back(u);
    };

    for (int i = 0; i < S; i++) {
        addEdge(source, i, minReviews);
    }

    for (int i = 0; i < S; i++) {
        for (int j = 0; j < R; j++) {
            if (j == excludedReviewerIndex) continue;
            if (subs[i].primary == revs[j].primary) {
                addEdge(i, S + j, 1);
            }
        }
    }

    for (int j = 0; j < R; j++) {
        if (j == excludedReviewerIndex) continue;
        addEdge(S + j, sink, maxReviews);
    }

    int totalFlow = edmondsKarp(capacity, adj, source, sink);

    if (assignments != nullptr) assignments->clear();
    if (missing != nullptr) missing->clear();

    for (int i = 0; i < S; i++) {
        int assignedToSubmission = 0;
        for (int j = 0; j < R; j++) {
            if (j == excludedReviewerIndex) continue;
            if (capacity[i][S + j] > 0) {
                assignedToSubmission += capacity[i][S + j];
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

/**
 * @brief Assigns reviewers to submissions based on primary domain matching.
 * 
 * This function distributes reviewers to submissions by matching their primary
 * domain. Each submission is assigned at least a minimum number of reviewers
 * (if possible), while ensuring that no reviewer exceeds the maximum allowed
 * number of reviews.
 * 
 * If a submission cannot receive the required number of reviewers, it is
 * recorded in the missing reviews list.
 * 
 * @param subs Vector of submissions to be assigned reviewers.
 * @param revs Vector of available reviewers.
 * @param params Map containing configuration parameters:
 *        - "MinReviewsPerSubmission": minimum number of reviewers per submission.
 *        - "MaxReviewsPerReviewer": maximum number of submissions per reviewer.
 * @param assignments Vector where the generated assignments will be stored.
 * @param missing Vector where missing review information will be stored.
 */
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

/**
 * @brief Writes reviewer assignments to an output file.
 * 
 * This function outputs the assignment results in a structured format:
 * - Submission-to-reviewer assignments
 * - Reviewer-to-submission mappings
 * - Total number of assignments
 * - Missing review information (if any)
 * 
 * It also prints warnings to the console if some submissions did not
 * receive the required number of reviews.
 * 
 * @param outputFileName Name of the output file where results will be written.
 * @param missing Vector containing submissions with missing reviews.
 */
void writeAssignments(const vector<Assignment> &assignments,const string &outputFileName,
                      const vector<MissingReviews> &missing) {
    fs::path resolvedOutputPath = resolveOutputPath(outputFileName);
    ofstream out(resolvedOutputPath);

    if (!out.is_open()) {
        cerr << "Error opening output file: " << resolvedOutputPath.string() << "\n";
        return;
    }

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

    out.close();

    cout << "Assignments written to " << resolvedOutputPath.string() << endl;
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
    fs::path resolvedOutputPath = resolveOutputPath(outputFileName);
    ofstream out(resolvedOutputPath);

    if (!out.is_open()) {
        cerr << "Error opening risk output file: " << resolvedOutputPath.string() << "\n";
        return;
    }

    out << "#CriticalReviewerId,SubmissionId,Domain,MissingReviews\n";

    if (criticalReviewers.empty()) {
        out << "#None\n";
    }
    else {
        for (const auto &risk : criticalReviewers) {
            for (const auto &entry : risk.missing) {
                out << risk.reviewerId << ", " << entry.submissionId << ", "
                    << entry.domain << ", " << entry.missing << "\n";
            }
        }
    }

    out.close();
    cout << "Risk analysis written to " << resolvedOutputPath.string() << endl;
}
