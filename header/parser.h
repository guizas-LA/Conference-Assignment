#ifndef PROJECT_PARSER_H
#define PROJECT_PARSER_H
#include <vector>
#include <map>
#include <string>
#include "structures.h"
using namespace std;

/**
 * @brief Reads and validates an input dataset file.
 *
 * The parser loads submissions, reviewers and control parameters from a CSV
 * file, supporting both explicit paths and project dataset paths.
 *
 * @param filename Name or path of the dataset file.
 * @param submissions Output vector with the parsed submissions.
 * @param reviewers Output vector with the parsed reviewers.
 * @param parameters Output map with parsed parameters and control values.
 * @return true if the file was successfully parsed and validated, false otherwise.
 *
 * @note Time complexity: O(L + S log S + R log R + P), where L is the number
 * of lines, S the number of submissions, R the number of reviewers and P the
 * number of parameters.
 */
bool readFile(const string &filename,vector<Submission> &submissions,vector<Reviewer> &reviewers,map<string, string> &parameters);

#endif //PROJECT_PARSER_H
