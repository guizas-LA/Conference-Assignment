#include "../header/batch.h"
#include "../header/parser.h"
#include "../header/assignment.h"
#include "../header/structures.h"
#include <iostream>
#include <fstream>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

/**
 * @brief Checks whether a control value is enabled.
 * @param params Parameter map.
 * @param key Parameter name.
 * @return true when the parameter exists and evaluates to a non-zero value.
 */
static bool controlFlagEnabled(const map<string,string> &params, const string &key) {
    auto it = params.find(key);
    if (it == params.end()) return false;

    try {
        return stoi(it->second) != 0;
    }
    catch (...) {
        return !it->second.empty() && it->second != "0";
    }
}

/**
 * @brief Parses an integer control parameter from the dataset.
 * @param params Parameter map.
 * @param key Parameter name.
 * @return Integer value, or 0 when missing or invalid.
 */
static int controlValue(const map<string,string> &params, const string &key) {
    auto it = params.find(key);
    if (it == params.end()) return 0;

    try {
        return stoi(it->second);
    }
    catch (...) {
        return 0;
    }
}

void batchMode(string inputFile, string riskFile) {
    vector<Submission> subs;
    vector<Reviewer> revs;
    map<string,string> params;

    if (!readFile(inputFile, subs, revs, params)) {
        cerr << "Error reading input file.\n";
        return;
    }

    fs::path resolvedOutputPath(resolveDatasetPath(riskFile, "output"));
    ofstream out(resolvedOutputPath);
    if (!out.is_open()) {
        cerr << "Error creating output file.\n";
        return;
    }

    int generateAssignments = controlValue(params, "GenerateAssignments");
    int riskAnalysis = controlValue(params, "RiskAnalysis");

    if (generateAssignments != 0) {
        vector<Assignment> assignments;
        vector<MissingReviews> missing;
        primaryAssignments(subs, revs, params, assignments, missing);
        writeAssignments(out, assignments, missing);
    }

    if (riskAnalysis != 0) {
        if (generateAssignments != 0) {
            out << "\n";
        }

        if (riskAnalysis == 1) {
            vector<RiskResult> criticalReviewers;
            bool baseFeasible = analyzeReviewerRisk(subs, revs, params, criticalReviewers);

            if (!baseFeasible) {
                out << "#BaseAssignmentInfeasible\n";
            }
            else {
                writeRiskAnalysis(out, criticalReviewers);
            }
        }
        else {
            vector<vector<int>> criticalReviewerSets;
            bool baseFeasible = analyzeReviewerRisk(subs, revs, params, riskAnalysis, criticalReviewerSets);

            if (!baseFeasible) {
                out << "#BaseAssignmentInfeasible\n";
            }
            else {
                writeRiskAnalysis(out, riskAnalysis, criticalReviewerSets);
            }
        }
    }

    out.close();
    cout << "Batch completed.\n";
}
