#ifndef PROJECT_ASSIGNMENT_H
#define PROJECT_ASSIGNMENT_H

#include "structures.h"
#include <vector>
#include <map>
#include <string>

using namespace std;

/**
 * @struct MissingReviews
 * @brief Stores information about missing reviews for a submission in a specific domain.
 */
struct MissingReviews {
    int submissionId; /**< ID of the submission */
    int domain;       /**< Domain identifier */
    int missing;      /**< Number of missing reviews */
};
struct Assignment {
    int submissionId;
    int reviewerId;
};

/**
 * @brief Assigns primary reviewers to submissions.
 *
 * This function distributes reviewers across submissions based on given parameters.
 * It also tracks any missing reviews per submission and domain.
 *
 * @param subs Vector of submissions to be reviewed (modified in-place).
 * @param revs Vector of reviewers available for assignment (modified in-place).
 * @param params Configuration parameters controlling the assignment process.
 * @param missing Output vector storing information about missing reviews.
 */
void primaryAssignments(vector<Submission> &subs,vector<Reviewer> &revs,const map<string,string> &params,vector<Assignment> &assignments);

/**
 * @brief Writes the reviewer assignments to an output file.
 *
 * Outputs the assignment results, including any missing reviews, into a file.
 *
 * @param subs Vector of submissions with assigned reviewers.
 * @param revs Vector of reviewers involved in the assignment.
 * @param outputFileName Name of the file where results will be written.
 * @param missing Vector containing information about missing reviews.
 */
void writeAssignments(const vector<Assignment> &assignments,const string &outputFileName);


#endif //PROJECT_ASSIGNMENT_H