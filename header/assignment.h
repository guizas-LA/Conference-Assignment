#ifndef PROJECT_ASSIGNMENT_H
#define PROJECT_ASSIGNMENT_H

#include "structures.h"
#include <vector>
#include <map>
#include <string>

using namespace std;

struct MissingReviews {
    int submissionId;
    int domain;
    int missing;
};

void primaryAssignments(vector<Submission> &subs , vector<Reviewer> &revs , const map<string,string> &params, vector<MissingReviews> &missing);
void writeAssignments(const vector<Submission> &subs , const vector<Reviewer> &revs , const string &outputFileName, const vector<MissingReviews> &missing);


#endif //PROJECT_ASSIGNMENT_H