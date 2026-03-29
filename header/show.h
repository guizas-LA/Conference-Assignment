#ifndef PROJECT_SHOW_H
#define PROJECT_SHOW_H
#include "../header/structures.h"
#include <vector>
#include <map>
#include <string>

/**
 * @brief Prints the list of loaded submissions to the terminal.
 * @param subs Vector of submissions to display.
 */
void showSubmissions(const std::vector<Submission> &subs);

/**
 * @brief Prints the list of loaded reviewers to the terminal.
 * @param revs Vector of reviewers to display.
 */
void showReviewers(const std::vector<Reviewer> &revs);

/**
 * @brief Prints the currently loaded control parameters to the terminal.
 * @param params Map containing the parsed parameter values.
 */
void showParameters(const std::map<std::string, std::string> &params);



#endif //PROJECT_SHOW_H
