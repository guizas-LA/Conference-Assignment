#include "../header/assignment.h"
#include "../header/maxflow.h"
#include <iostream>
#include <fstream>

using namespace std;

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
 * @param missing Vector where missing review information will be stored.
 */
void primaryAssignments(vector<Submission> &subs,vector<Reviewer> &revs,const map<string,string> &params,vector<Assignment> &assignments){

    assignments.clear();

    int minReviews = stoi(params.at("MinReviewsPerSubmission"));
    int maxReviews = stoi(params.at("MaxReviewsPerReviewer"));

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
            if (subs[i].primary == revs[j].primary) {
                addEdge(i, S + j, 1);
            }
        }
    }
    for (int j = 0; j < R; j++) {
        addEdge(S + j, sink, maxReviews);
    }

    int totalFlow = edmondsKarp(capacity, adj, source, sink);

    for (int i = 0; i < S; i++) {
        for (int j = 0; j < R; j++) {
            if (capacity[i][S + j] > 0) {
                assignments.push_back({subs[i].id, revs[j].id});
            }
        }
    }
    cout << "Assignments generated using MaxFlow.\n";
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
 * @param subs Vector of submissions with assigned reviewers.
 * @param revs Vector of reviewers.
 * @param outputFileName Name of the output file where results will be written.
 * @param missing Vector containing submissions with missing reviews.
 */
void writeAssignments(const vector<Assignment> &assignments,const string &outputFileName) {
    ofstream out(outputFileName);

    out << "#SubmissionId,ReviewerId\n";

    for (const auto &a : assignments) {
        out << a.submissionId << "," << a.reviewerId << "\n";
    }

    out << "#Total: " << assignments.size() << "\n";

    out.close();

    cout << "Assignments written to " << outputFileName << endl;
}