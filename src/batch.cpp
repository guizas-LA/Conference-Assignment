/**
 * @file batch.cpp
 * @brief Batch mode implementation for automated processing.
 * @author Conference Organization System
 */

#include "../header/batch.h"
#include "../header/parser.h"
#include "../header/assignment.h"
#include "../header/structures.h"
#include <iostream>
#include <fstream>
using namespace std;


void batchMode(string inputFile, string riskFile) {
    vector<Submission> subs;
    vector<Reviewer> revs;
    map<string,string> params;

    if (!readFile(inputFile, subs, revs, params)) {
        cerr << "Error reading input file.\n";
        return;
    }

    vector<RiskResult> criticalReviewers;
    bool baseFeasible = analyzeReviewerRisk(subs, revs, params, criticalReviewers);

    if (!baseFeasible) {
        ofstream out(riskFile);
        if (!out.is_open()) {
            cerr << "Error creating output file.\n";
            return;
        }

        out << "#BaseAssignmentInfeasible\n";
        out.close();
        cout << "Batch completed.\n";
        return;
    }

    writeRiskAnalysis(criticalReviewers, riskFile);
    cout << "Batch completed.\n";
}
