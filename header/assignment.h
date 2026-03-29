#ifndef PROJECT_ASSIGNMENT_H
#define PROJECT_ASSIGNMENT_H

#include "structures.h"
#include <vector>
#include <map>
#include <string>
#include <iosfwd>

using namespace std;

/**
 * @struct MissingReviews
 * @brief Stores information about missing reviews for a submission in a specific domain.
 */
struct MissingReviews {
    int submissionId; /**< ID of the submission */
    int domain;       /**< Required domain expertise */
    int missing;      /**< Number of missing reviews */
};

/**
 * @struct Assignment
 * @brief Represents a successful match between a submission and a reviewer.
 */
struct Assignment {
    int submissionId;
    int reviewerId;
    int match;
};

/**
 * @struct RiskResult
 * @brief Stores the missing reviews caused by the failure of a specific reviewer.
 */
struct RiskResult {
    int reviewerId;
    vector<MissingReviews> missing;
};

/**
 * @brief Computes review assignments according to the configured assignment mode.
 *
 * This function distributes reviewers across submissions based on the selected
 * matching mode and also tracks any missing reviews per submission.
 *
 * @param subs Vector of submissions to be reviewed (modified in-place).
 * @param revs Vector of reviewers available for assignment (modified in-place).
 * @param params Configuration parameters controlling the assignment process.
 * @param assignments Output vector storing the generated assignment pairs.
 * @param missing Output vector storing information about missing reviews.
 *
 * @note Time complexity: dominated by Edmonds-Karp, O(V * E^2).
 */
void primaryAssignments(vector<Submission> &subs,vector<Reviewer> &revs,const map<string,string> &params,
                        vector<Assignment> &assignments,vector<MissingReviews> &missing);

/**
 * @brief Writes the reviewer assignments to an output file.
 *
 * Outputs the assignment results, including any missing reviews, into a file.
 *
 * @param assignments Vector of generated assignment pairs.
 * @param outputFileName Name of the file where results will be written.
 * @param missing Vector containing information about missing reviews.
 */
void writeAssignments(const vector<Assignment> &assignments,const string &outputFileName,
                      const vector<MissingReviews> &missing);

void writeAssignments(ostream &out,const vector<Assignment> &assignments,
                      const vector<MissingReviews> &missing);

/**
 * @brief Resolves a dataset-relative path for a given folder name.
 * @param fileName File name to resolve.
 * @param folderName Target dataset subfolder.
 * @return The resolved file path.
 */
string resolveDatasetPath(const string &fileName, const string &folderName);

/**
 * @brief Performs risk analysis for the failure of any single reviewer.
 *
 * @param subs Parsed submissions.
 * @param revs Parsed reviewers.
 * @param params Parsed parameters.
 * @param criticalReviewers Output vector with reviewers whose removal makes the assignment infeasible.
 * @return true if the base assignment is feasible, false otherwise.
 *
 * @note Time complexity: O(R * V * E^2), where R is the number of reviewers.
 */
bool analyzeReviewerRisk(const vector<Submission> &subs,const vector<Reviewer> &revs,
                         const map<string,string> &params,vector<RiskResult> &criticalReviewers);

/**
 * @brief Performs generalized risk analysis for the failure of any K reviewers.
 *
 * @param subs Parsed submissions.
 * @param revs Parsed reviewers.
 * @param params Parsed parameters.
 * @param riskLevel Number of simultaneous reviewer failures to test.
 * @param criticalReviewerSets Output vector with critical reviewer id sets.
 * @return true if the base assignment is feasible, false otherwise.
 *
 * @note Time complexity: O(C(R,K) * V * E^2).
 */
bool analyzeReviewerRisk(const vector<Submission> &subs,const vector<Reviewer> &revs,
                         const map<string,string> &params,int riskLevel,
                         vector<vector<int>> &criticalReviewerSets);

/**
 * @brief Writes single-reviewer risk analysis results to an output file.
 * @param criticalReviewers Vector with critical reviewers.
 * @param outputFileName Output file name.
 */
void writeRiskAnalysis(const vector<RiskResult> &criticalReviewers,const string &outputFileName);

/**
 * @brief Writes single-reviewer risk analysis results to an output stream.
 * @param out Destination output stream.
 * @param criticalReviewers Vector with critical reviewers.
 */
void writeRiskAnalysis(ostream &out,const vector<RiskResult> &criticalReviewers);

/**
 * @brief Writes generalized K-reviewer risk analysis results to an output stream.
 * @param out Destination output stream.
 * @param riskLevel Risk analysis level K.
 * @param criticalReviewerSets Vector with critical reviewer id sets.
 */
void writeRiskAnalysis(ostream &out,int riskLevel,const vector<vector<int>> &criticalReviewerSets);


#endif //PROJECT_ASSIGNMENT_H
