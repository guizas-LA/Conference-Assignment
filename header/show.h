#ifndef PROJECT_SHOW_H
#define PROJECT_SHOW_H
#include "../header/structures.h"
#include <vector>
#include <map>
#include <string>


void showSubmissions(const std::vector<Submission> &subs);
void showReviewers(const std::vector<Reviewer> &revs);
void showParameters(const std::map<std::string, std::string> &params);



#endif //PROJECT_SHOW_H