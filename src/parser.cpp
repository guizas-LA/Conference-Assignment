#include "../header/parser.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
using namespace std;
namespace fs = std::filesystem;

/**
 * @brief Removes leading and trailing whitespace from a string.
 * @param s String to normalize.
 * @return Trimmed string.
 */
string trim(string s) {
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    s.erase(s.find_last_not_of(" \t\r\n") + 1);
    return s;
}

/**
 * @brief Removes surrounding quotes from a CSV field when present.
 * @param s Raw field string.
 * @return Unquoted field value.
 */
static string stripQuotes(string s) {
    s = trim(s);
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

/**
 * @brief Splits a CSV line using a delimiter.
 * @param line Input line.
 * @param delimiter Separator character.
 * @return Vector with the parsed fields.
 */
vector<string> split(const string &line, char delimiter) {
    vector<string> parts;
    string part;
    stringstream ss(line);

    while (getline(ss, part, delimiter)) {
        while (!part.empty() && part.front() == ' ') part.erase(0, 1);
        while (!part.empty() && part.back() == ' ') part.pop_back();
        parts.push_back(part);
    }

    return parts;
}

/**
 * @brief Checks whether a required parameter exists in the parameter map.
 * @param parameters Parsed parameters.
 * @param key Parameter name.
 * @return true if the parameter exists, false otherwise.
 */
static bool requireParameter(const map<string, string> &parameters, const string &key) {
    return parameters.find(key) != parameters.end();
}

/**
 * @brief Validates the parsed dataset control parameters.
 * @param submissions Parsed submissions.
 * @param reviewers Parsed reviewers.
 * @param parameters Parsed parameters.
 * @return true if the dataset is valid, false otherwise.
 */
static bool validateParameters(const vector<Submission> &submissions,
                               const vector<Reviewer> &reviewers,
                               const map<string, string> &parameters) {
    const vector<string> requiredKeys = {
            "MinReviewsPerSubmission",
            "MaxReviewsPerReviewer",
            "GenerateAssignments",
            "RiskAnalysis"
    };

    for (const auto &key : requiredKeys) {
        if (!requireParameter(parameters, key)) {
            cerr << "Missing required parameter: " << key << "\n";
            return false;
        }
    }

    try {
        int minReviews = stoi(parameters.at("MinReviewsPerSubmission"));
        int maxReviews = stoi(parameters.at("MaxReviewsPerReviewer"));
        int generateAssignments = stoi(parameters.at("GenerateAssignments"));
        int riskAnalysis = stoi(parameters.at("RiskAnalysis"));

        if (minReviews <= 0) {
            cerr << "MinReviewsPerSubmission must be greater than 0.\n";
            return false;
        }

        if (maxReviews <= 0) {
            cerr << "MaxReviewsPerReviewer must be greater than 0.\n";
            return false;
        }

        if (generateAssignments < 0 || generateAssignments > 3) {
            cerr << "GenerateAssignments must be between 0 and 3.\n";
            return false;
        }

        if (riskAnalysis < 0 || riskAnalysis > static_cast<int>(reviewers.size())) {
            cerr << "RiskAnalysis must be between 0 and the number of reviewers.\n";
            return false;
        }
    }
    catch (const exception &e) {
        cerr << "Invalid numeric parameter value. Reason: " << e.what() << "\n";
        return false;
    }

    if (submissions.empty()) {
        cerr << "Dataset must contain at least one submission.\n";
        return false;
    }

    if (reviewers.empty()) {
        cerr << "Dataset must contain at least one reviewer.\n";
        return false;
    }

    return true;
}

bool readFile(const string &filename,vector<Submission> &submissions,vector<Reviewer> &reviewers,map<string, string> &parameters) {
    fs::path requested(filename);
    if (!requested.has_extension()) {
        requested += ".csv";
    }

    vector<fs::path> candidates;
    fs::path current = fs::current_path();

    if (requested.is_absolute()) {
        candidates.push_back(requested);
    }
    else {
        candidates.push_back(current / requested);
        candidates.push_back(current / "dataset" / "input" / requested);
        candidates.push_back(current / "dataset" / requested);
        candidates.push_back(current.parent_path() / requested);
        candidates.push_back(current.parent_path() / "dataset" / "input" / requested);
        candidates.push_back(current.parent_path() / "dataset" / requested);
    }

    ifstream file;
    for (const auto &candidate : candidates) {
        file.open(candidate);
        if (file.is_open()) {
            break;
        }
        file.clear();
    }

    if (!file.is_open()) {
        cerr << "Error opening file: " << requested.string() << "\n";
        return false;
    }

    submissions.clear();
    reviewers.clear();
    parameters.clear();

    string line;
    string section = "";
    set<int> submissionIds;
    set<int> reviewerIds;

    while (getline(file, line)) {
        line = trim(line);

        if (line.empty()) continue;

        if (line.find("#Submissions") != string::npos) {
            section = "submissions";
            continue;
        }
        if (line.find("#Reviewers") != string::npos) {
            section = "reviewers";
            continue;
        }
        if (line.find("#Parameters") != string::npos) {
            section = "parameters";
            continue;
        }

        size_t commentPos = line.find('#');
        if (commentPos != string::npos) {
            line = trim(line.substr(0, commentPos));
            if (line.empty()) continue;
        }

        if (line[0] == '#') continue;

        vector<string> parts = split(line, ',');

        for (auto &p : parts) {
            p = trim(p);
        }

        try {
            if (section == "submissions") {

                if (parts.size() < 5) continue;

                Submission s;
                s.id = stoi(parts[0]);
                s.title = parts[1];
                s.authors = parts[2];
                s.email = parts[3];
                s.primary = stoi(parts[4]);

                if (parts.size() > 5 && parts[5] != "") {
                    s.secondary = stoi(parts[5]);
                }
                else {
                    s.secondary = -1;
                }

                if (!submissionIds.insert(s.id).second) {
                    cerr << "Duplicate submission id: " << s.id << "\n";
                    return false;
                }

                submissions.push_back(s);
            }
            else if (section == "reviewers") {

                if (parts.size() < 4) continue;

                Reviewer r;
                r.id = stoi(parts[0]);
                r.name = parts[1];
                r.email = parts[2];
                r.primary = stoi(parts[3]);

                if (parts.size() > 4 && parts[4] != "") {
                    r.secondary = stoi(parts[4]);
                } else {
                    r.secondary = -1;
                }

                if (!reviewerIds.insert(r.id).second) {
                    cerr << "Duplicate reviewer id: " << r.id << "\n";
                    return false;
                }

                reviewers.push_back(r);
            }
            else if (section == "parameters") {

                if (parts.size() >= 2) {
                    parameters[parts[0]] = stripQuotes(parts[1]);
                }
            }
        }
        catch (exception &e) {
            cerr << "Error processing line:" << line << endl;
            cerr << "Reason: " << e.what() << endl;
            return false;
        }
    }

    file.close();
    return validateParameters(submissions, reviewers, parameters);
}
