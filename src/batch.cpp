#include "../header/batch.h"
#include "../header/parser.h"
#include "../header/assignment.h"
#include "../header/structures.h"
#include <iostream>
#include <fstream>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

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

    bool generateAssignments = controlFlagEnabled(params, "GenerateAssignments");
    bool runRiskAnalysis = controlFlagEnabled(params, "RiskAnalysis");

    if (generateAssignments) {
        vector<Assignment> assignments;
        vector<MissingReviews> missing;
        primaryAssignments(subs, revs, params, assignments, missing);
        writeAssignments(out, assignments, missing);
    }

    if (runRiskAnalysis) {
        if (generateAssignments) {
            out << "\n";
        }

        vector<RiskResult> criticalReviewers;
        bool baseFeasible = analyzeReviewerRisk(subs, revs, params, criticalReviewers);

        if (!baseFeasible) {
            out << "#BaseAssignmentInfeasible\n";
        }
        else {
            writeRiskAnalysis(out, criticalReviewers);
        }
    }

    out.close();
    cout << "Batch completed.\n";
}
