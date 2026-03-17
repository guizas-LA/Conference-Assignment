#ifndef PROJECT_PARSER_H
#define PROJECT_PARSER_H
#include <vector>
#include <map>
#include <string>
#include "structures.h"
using namespace std;


bool readFile(const string &filename,vector<Submission> &submissions,vector<Reviewer> &reviewers,map<string, string> &parameters);

#endif //PROJECT_PARSER_H