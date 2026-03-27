/**
 * @file parser.h
 * @brief File parsing utilities for reading CSV dataset files.
 * @author Conference Organization System
 */

#ifndef PROJECT_PARSER_H
#define PROJECT_PARSER_H
#include <vector>
#include <map>
#include <string>
#include "structures.h"
using namespace std;

/**
 * @brief Reads and parses a CSV data file containing submissions, reviewers, and parameters.
 * 
 * Loads data from a CSV file with three main sections (#Submissions, #Reviewers, #Parameters).
 * Lines starting with # (except section headers) and empty lines are ignored.
 * Comments after # on a line are preserved in data fields.
 * 
 * @param filename Name of the CSV file to read (looked up in data/ or ../data/ directories).
 * @param submissions Output vector populated with parsed submission data.
 * @param reviewers Output vector populated with parsed reviewer data.
 * @param parameters Output map populated with configuration parameters.
 * @return true if file was successfully parsed, false if file could not be opened or parse error occurred.
 * 
 * @note Time Complexity: O(n) where n is the total number of lines/entries in the file.
 *       Space Complexity: O(s + r + p) where s is number of submissions, r is number of reviewers, p is number of parameters.
 */
bool readFile(const string &filename,vector<Submission> &submissions,vector<Reviewer> &reviewers,map<string, string> &parameters);

#endif //PROJECT_PARSER_H