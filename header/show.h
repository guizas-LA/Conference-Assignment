/**
 * @file show.h
 * @brief Display utilities for presenting data to the user.
 * @author Conference Organization System
 */

#ifndef PROJECT_SHOW_H
#define PROJECT_SHOW_H
#include "../header/structures.h"
#include <vector>
#include <map>
#include <string>

/**
 * @brief Displays all loaded submissions in a formatted table.
 * 
 * Prints submission information including ID, title, authors, email,
 * and primary/secondary domains to the console.
 * 
 * @param subs Vector of submissions to display.
 * 
 * @note Time Complexity: O(n) where n is the number of submissions.
 */
void showSubmissions(const std::vector<Submission> &subs);

/**
 * @brief Displays all loaded reviewers in a formatted table.
 * 
 * Prints reviewer information including ID, name, email,
 * and primary/secondary expertise domains to the console.
 * 
 * @param revs Vector of reviewers to display.
 * 
 * @note Time Complexity: O(r) where r is the number of reviewers.
 */
void showReviewers(const std::vector<Reviewer> &revs);

/**
 * @brief Displays all configuration parameters.
 * 
 * Prints all parameter key-value pairs to the console in a formatted manner.
 * 
 * @param params Map of parameter names and their values.
 * 
 * @note Time Complexity: O(p) where p is the number of parameters.
 */
void showParameters(const std::map<std::string, std::string> &params);



#endif //PROJECT_SHOW_H