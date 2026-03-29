#include "../header/assignment.h"
#include "../header/maxflow.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <set>

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

/**
 * @brief Collects the submission domains allowed by a given assignment mode.
 * @param sub Submission being matched.
 * @param assignmentMode Assignment mode from the control section.
 * @return Ordered list of valid submission domains.
 */
static vector<int> submissionDomains(const Submission &sub, int assignmentMode) {
    vector<int> domains = {sub.primary};
    if (assignmentMode >= 2 && sub.secondary != -1 && sub.secondary != sub.primary) {
        domains.push_back(sub.secondary);
    }
    return domains;
}

/**
 * @brief Collects the reviewer domains allowed by a given assignment mode.
 * @param rev Reviewer being matched.
 * @param assignmentMode Assignment mode from the control section.
 * @return Ordered list of valid reviewer domains.
 */
static vector<int> reviewerDomains(const Reviewer &rev, int assignmentMode) {
    vector<int> domains = {rev.primary};
    if (assignmentMode >= 3 && rev.secondary != -1 && rev.secondary != rev.primary) {
        domains.push_back(rev.secondary);
    }
    return domains;
}

/**
 * @brief Determines the first valid matching domain between a submission and a reviewer.
 * @param sub Submission being analyzed.
 * @param rev Reviewer being analyzed.
 * @param assignmentMode Assignment mode from the control section.
 * @return Matching domain id, or -1 when no match exists.
 */
static int matchDomain(const Submission &sub, const Reviewer &rev, int assignmentMode) {
    vector<int> subDomains = submissionDomains(sub, assignmentMode);
    vector<int> revDomains = reviewerDomains(rev, assignmentMode);

    for (int subDomain : subDomains) {
        for (int revDomain : revDomains) {
            if (subDomain == revDomain) {
                return subDomain;
            }
        }
    }

    return -1;
}

/**
 * @brief Builds and solves the review assignment flow network.
 *
 * The function supports the different assignment modes and can exclude any set
 * of reviewers for risk analysis.
 *
 * @param subs Parsed submissions.
 * @param revs Parsed reviewers.
 * @param assignmentMode Assignment mode from the control section.
 * @param minReviews Minimum reviews required per submission.
 * @param maxReviews Maximum reviews allowed per reviewer.
 * @param excludedReviewerIndices Reviewer indices temporarily removed from the network.
 * @param assignments Optional output vector with successful assignments.
 * @param missing Optional output vector with missing review information.
 * @return Total flow found in the network.
 *
 * @note Time complexity: O(V * E^2).
 */
static int solveAssignmentFlow(const vector<Submission> &subs,const vector<Reviewer> &revs,
                               int assignmentMode,int minReviews,int maxReviews,
                               const set<int> &excludedReviewerIndices,
                               vector<Assignment> *assignments,vector<MissingReviews> *missing) {
    int submissionCount = subs.size();
    int reviewerCount = revs.size();

    int source = submissionCount + reviewerCount;
    int sink = submissionCount + reviewerCount + 1;
    int nodeCount = submissionCount + reviewerCount + 2;

    vector<vector<int>> capacity(nodeCount, vector<int>(nodeCount, 0));
    vector<vector<int>> adj(nodeCount);
    vector<vector<int>> edgeMatchDomain(submissionCount, vector<int>(reviewerCount, -1));

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
            if (excludedReviewerIndices.count(j) != 0) continue;

            int domain = matchDomain(subs[i], revs[j], assignmentMode);
            if (domain != -1) {
                addEdge(i, submissionCount + j, 1);
                edgeMatchDomain[i][j] = domain;
            }
        }
    }

    for (int j = 0; j < reviewerCount; j++) {
        if (excludedReviewerIndices.count(j) != 0) continue;
        addEdge(submissionCount + j, sink, maxReviews);
    }

    int totalFlow = edmondsKarp(capacity, adj, source, sink);

    if (assignments != nullptr) assignments->clear();
    if (missing != nullptr) missing->clear();

    for (int i = 0; i < submissionCount; i++) {
        int assignedToSubmission = 0;
        for (int j = 0; j < reviewerCount; j++) {
            if (excludedReviewerIndices.count(j) != 0) continue;
            if (capacity[i][submissionCount + j] > 0) {
                assignedToSubmission += capacity[i][submissionCount + j];
                if (assignments != nullptr) {
                    assignments->push_back({subs[i].id, revs[j].id, edgeMatchDomain[i][j]});
                }
            }
        }

        if (missing != nullptr && assignedToSubmission < minReviews) {
            vector<int> domains = submissionDomains(subs[i], assignmentMode);
            int reportedDomain = domains.front();
            if (domains.size() > 1) {
                reportedDomain = domains.back();
            }
            missing->push_back({subs[i].id, reportedDomain, minReviews - assignedToSubmission});
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
    int assignmentMode = stoi(params.at("GenerateAssignments"));
    set<int> excludedReviewerIndices;
    solveAssignmentFlow(subs, revs, assignmentMode, minReviews, maxReviews,
                        excludedReviewerIndices, &assignments, &missing);

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
    vector<Assignment> submissionSorted = assignments;
    sort(submissionSorted.begin(), submissionSorted.end(),
         [](const Assignment &lhs, const Assignment &rhs) {
             if (lhs.submissionId != rhs.submissionId) return lhs.submissionId < rhs.submissionId;
             if (lhs.reviewerId != rhs.reviewerId) return lhs.reviewerId < rhs.reviewerId;
             return lhs.match < rhs.match;
         });

    out << "#SubmissionId,ReviewerId,Match\n";

    for (const auto &a : submissionSorted) {
        out << a.submissionId << ", " << a.reviewerId << ", " << a.match << "\n";
    }

    vector<Assignment> reviewerSorted = submissionSorted;
    sort(reviewerSorted.begin(), reviewerSorted.end(),
         [](const Assignment &lhs, const Assignment &rhs) {
             if (lhs.reviewerId != rhs.reviewerId) return lhs.reviewerId < rhs.reviewerId;
             if (lhs.submissionId != rhs.submissionId) return lhs.submissionId < rhs.submissionId;
             return lhs.match < rhs.match;
         });

    out << "#ReviewerId,SubmissionId,Match\n";

    for (const auto &a : reviewerSorted) {
        out << a.reviewerId << ", " << a.submissionId << ", " << a.match << "\n";
    }

    out << "#Total: " << assignments.size() << "\n";

    if (!missing.empty()) {
        vector<MissingReviews> sortedMissing = missing;
        sort(sortedMissing.begin(), sortedMissing.end(),
             [](const MissingReviews &lhs, const MissingReviews &rhs) {
                 if (lhs.submissionId != rhs.submissionId) return lhs.submissionId < rhs.submissionId;
                 if (lhs.domain != rhs.domain) return lhs.domain < rhs.domain;
                 return lhs.missing < rhs.missing;
             });

        out << "#SubmissionId,Domain,MissingReviews\n";

        for (const auto &entry : sortedMissing) {
            out << entry.submissionId << ", " << entry.domain << ", " << entry.missing << "\n";
        }
    }
}

bool analyzeReviewerRisk(const vector<Submission> &subs,const vector<Reviewer> &revs,
                         const map<string,string> &params,vector<RiskResult> &criticalReviewers) {
    criticalReviewers.clear();

    int minReviews = stoi(params.at("MinReviewsPerSubmission"));
    int maxReviews = stoi(params.at("MaxReviewsPerReviewer"));
    int assignmentMode = stoi(params.at("GenerateAssignments"));
    int requiredFlow = static_cast<int>(subs.size()) * minReviews;

    vector<Assignment> baselineAssignments;
    vector<MissingReviews> baselineMissing;
    set<int> excludedReviewerIndices;
    int baselineFlow = solveAssignmentFlow(subs, revs, assignmentMode, minReviews, maxReviews,
                                           excludedReviewerIndices, &baselineAssignments, &baselineMissing);

    if (baselineFlow < requiredFlow) {
        return false;
    }

    for (int reviewerIndex = 0; reviewerIndex < static_cast<int>(revs.size()); reviewerIndex++) {
        vector<MissingReviews> missing;
        set<int> excluded = {reviewerIndex};
        int flowWithoutReviewer = solveAssignmentFlow(subs, revs, assignmentMode, minReviews, maxReviews,
                                                      excluded, nullptr, &missing);

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

/**
 * @brief Generates all reviewer index combinations of a fixed size.
 * @param reviewerCount Total number of reviewers.
 * @param combinationSize Required combination size.
 * @param start Next index to consider.
 * @param current Combination currently being built.
 * @param combinations Output vector with all generated combinations.
 *
 * @note Time complexity: O(C(R,K) * K).
 */
static void generateReviewerCombinations(int reviewerCount, int combinationSize, int start,
                                         vector<int> &current, vector<vector<int>> &combinations) {
    if (static_cast<int>(current.size()) == combinationSize) {
        combinations.push_back(current);
        return;
    }

    for (int i = start; i <= reviewerCount - (combinationSize - static_cast<int>(current.size())); i++) {
        current.push_back(i);
        generateReviewerCombinations(reviewerCount, combinationSize, i + 1, current, combinations);
        current.pop_back();
    }
}

bool analyzeReviewerRisk(const vector<Submission> &subs,const vector<Reviewer> &revs,
                         const map<string,string> &params,int riskLevel,
                         vector<vector<int>> &criticalReviewerSets) {
    criticalReviewerSets.clear();

    if (riskLevel <= 1) {
        vector<RiskResult> criticalReviewers;
        bool feasible = analyzeReviewerRisk(subs, revs, params, criticalReviewers);
        for (const auto &risk : criticalReviewers) {
            criticalReviewerSets.push_back({risk.reviewerId});
        }
        return feasible;
    }

    int minReviews = stoi(params.at("MinReviewsPerSubmission"));
    int maxReviews = stoi(params.at("MaxReviewsPerReviewer"));
    int assignmentMode = stoi(params.at("GenerateAssignments"));
    int requiredFlow = static_cast<int>(subs.size()) * minReviews;

    vector<Assignment> baselineAssignments;
    vector<MissingReviews> baselineMissing;
    set<int> noExcluded;
    int baselineFlow = solveAssignmentFlow(subs, revs, assignmentMode, minReviews, maxReviews,
                                           noExcluded, &baselineAssignments, &baselineMissing);
    if (baselineFlow < requiredFlow) {
        return false;
    }

    vector<vector<int>> combinations;
    vector<int> current;
    generateReviewerCombinations(static_cast<int>(revs.size()), riskLevel, 0, current, combinations);

    for (const auto &combination : combinations) {
        set<int> excluded(combination.begin(), combination.end());
        int flowWithoutReviewers = solveAssignmentFlow(subs, revs, assignmentMode, minReviews, maxReviews,
                                                       excluded, nullptr, nullptr);
        if (flowWithoutReviewers < requiredFlow) {
            vector<int> reviewerIds;
            for (int reviewerIndex : combination) {
                reviewerIds.push_back(revs[reviewerIndex].id);
            }
            sort(reviewerIds.begin(), reviewerIds.end());
            criticalReviewerSets.push_back(reviewerIds);
        }
    }

    sort(criticalReviewerSets.begin(), criticalReviewerSets.end());
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

void writeRiskAnalysis(ostream &out,int riskLevel,const vector<vector<int>> &criticalReviewerSets) {
    out << "#Risk Analysis: " << riskLevel << "\n";

    if (criticalReviewerSets.empty()) {
        out << "\n";
        return;
    }

    for (const auto &reviewerSet : criticalReviewerSets) {
        for (size_t i = 0; i < reviewerSet.size(); i++) {
            if (i > 0) out << ", ";
            out << reviewerSet[i];
        }
        out << "\n";
    }
}
