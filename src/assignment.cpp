#include "../header/assignment.h"
#include "../header/maxflow.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

/**
 * @brief Solves the primary domain flow problem using Edmonds-Karp.
 * 
 * Constructs a flow network where:
 * - Source connects to submissions with capacity = MinReviewsPerSubmission
 * - Submissions connect to reviewers if primary domain matches (capacity = 1)
 * - Reviewers connect to sink with capacity = MaxReviewsPerReviewer
 * 
 * Optionally excludes a specific reviewer (for risk analysis).
 * 
 * @param subs Vector of submissions.
 * @param revs Vector of reviewers.
 * @param minReviews Minimum reviews per submission.
 * @param maxReviews Maximum reviews per reviewer.
 * @param excludedReviewerIndex Index of reviewer to exclude (-1 for none).
 * @param assignments Pointer to output vector for assignments (nullptr to skip).
 * @param missing Pointer to output vector for missing reviews (nullptr to skip).
 * @return Total flow value achieved.
 * 
 * @note Time Complexity: O(V*E^2) where V = S+R+2 and E ≈ S*R, so O((S*R)^2 * (S+R)).
 *       Space Complexity: O((S+R)^2) for capacity and flow matrices.
 */
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
    ofstream out(outputFileName);

    if (!out.is_open()) {
        cerr << "Error opening output file: " << outputFileName << "\n";
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

    cout << "Assignments written to " << outputFileName << endl;
}

/**
 * @brief Analyzes which reviewers are critical to the assignment's feasibility (Risk Analysis = 1).
 * 
 * Determines which individual reviewers, if they fail to complete their reviews,
 * would make it impossible to satisfy the minimum reviews per submission requirement.
 * 
 * @param subs Vector of submissions.
 * @param revs Vector of reviewers.
 * @param params Map containing MinReviewsPerSubmission and MaxReviewsPerReviewer.
 * @param criticalReviewers Output vector of reviewers whose absence breaks the assignment.
 * @return true if base assignment is feasible, false otherwise.
 * 
 * @note Time Complexity: O(R * V*E^2) where R is number of reviewers.
 *       For each reviewer, runs full max flow algorithm, so roughly O(R * (S*R)^2 * (S+R)).
 */
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

/**
 * @brief Writes risk analysis results to an output file.
 * 
 * Outputs the list of critical reviewers whose absence would break the assignment,
 * including details about which submissions would be affected and missing reviews.
 * 
 * @param criticalReviewers Vector of reviewers critical to assignment feasibility.
 * @param outputFileName Name of the output file.
 * 
 * @note Time Complexity: O(CR) where C is number of critical reviewers and R is number of affected submissions.
 */
void writeRiskAnalysis(const vector<RiskResult> &criticalReviewers,const string &outputFileName) {
    ofstream out(outputFileName);

    if (!out.is_open()) {
        cerr << "Error opening risk output file: " << outputFileName << "\n";
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
    cout << "Risk analysis written to " << outputFileName << endl;
}
