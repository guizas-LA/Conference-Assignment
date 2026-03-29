#include "../header/menu.h"
#include "../header/parser.h"
#include "../header/structures.h"
#include <iostream>
#include "../header/assignment.h"
#include "../header/show.h"
using namespace std;

/**
 * @brief Runs the interactive text menu of the application.
 */
void menu() {
    int option;
    vector<Submission> submissions;
    vector<Reviewer> reviewers;
    map<string, string> parameters;
    string filename;

    do {
        cout << "=======================================\n";
        cout << "|     Scientific Conference Tool      |\n";
        cout << "=======================================\n";
        cout << "| 1 - Load dataset                    |\n";
        cout << "| 2 - List submissions                |\n";
        cout << "| 3 - List reviewers                  |\n";
        cout << "| 4 - List parameters                 |\n";
        cout << "| 5 - Generate assignments            |\n";
        cout << "| 6 - Risk analysis                   |\n";
        cout << "| 0 - Exit                            |\n";
        cout << "=======================================\n";
        cout << "Option: ";

        cin >> option;

        switch(option) {
            case 1:
                cout << "File name (without .csv): ";
                cin >> filename;
                if (readFile(filename, submissions, reviewers, parameters)) {
                    cout << "\nFile loaded successfully!\n";
                    cout << "Submissions: " << submissions.size() << endl;
                    cout << "Reviewers: " << reviewers.size() << endl;
                    cout << "Parameters: " << parameters.size() << endl;
                }
                break;

            case 2:
                showSubmissions(submissions);
                break;

            case 3:
                showReviewers(reviewers);
                break;

            case 4:
                showParameters(parameters);
                break;

            case 5:
                cout << "\nGenerating assignments...\n";

                if (submissions.empty() || reviewers.empty()) {
                    cout << "Load a dataset first!\n";
                }
                else {
                    vector<Assignment> assignments;
                    vector<MissingReviews> missing;
                    primaryAssignments(submissions, reviewers, parameters, assignments, missing);
                    string outputFile;
                    if (parameters.count("OutputFileName")) {
                        outputFile = parameters["OutputFileName"];
                    } else {
                        outputFile = "output.csv";
                    }
                    writeAssignments(assignments, outputFile, missing);
                }
                break;

            case 6:
                cout << "\nRunning risk analysis...\n";

                if (submissions.empty() || reviewers.empty()) {
                    cout << "Load a dataset first!\n";
                }
                else {
                    int riskLevel = 0;
                    if (parameters.count("RiskAnalysis")) {
                        riskLevel = stoi(parameters.at("RiskAnalysis"));
                    }

                    if (riskLevel <= 1) {
                        vector<RiskResult> criticalReviewers;
                        bool baseFeasible = analyzeReviewerRisk(submissions, reviewers, parameters, criticalReviewers);

                        if (!baseFeasible) {
                            cout << "Base assignment is already infeasible, so reviewer failure risk is not analyzed.\n";
                        }
                        else if (criticalReviewers.empty()) {
                            cout << "No single reviewer failure makes the assignment infeasible.\n";
                        }
                        else {
                            cout << "Critical reviewers found:\n";
                            for (const auto &risk : criticalReviewers) {
                                cout << "Reviewer " << risk.reviewerId << " is critical.\n";
                                for (const auto &entry : risk.missing) {
                                    cout << "  Submission " << entry.submissionId
                                         << " would still need " << entry.missing
                                         << " review(s) in domain " << entry.domain << ".\n";
                                }
                            }
                        }
                    }
                    else {
                        vector<vector<int>> criticalReviewerSets;
                        bool baseFeasible = analyzeReviewerRisk(submissions, reviewers, parameters,
                                                                riskLevel, criticalReviewerSets);

                        if (!baseFeasible) {
                            cout << "Base assignment is already infeasible, so reviewer failure risk is not analyzed.\n";
                        }
                        else if (criticalReviewerSets.empty()) {
                            cout << "No set of " << riskLevel
                                 << " reviewers makes the assignment infeasible.\n";
                        }
                        else {
                            cout << "Critical reviewer sets found:\n";
                            for (const auto &reviewerSet : criticalReviewerSets) {
                                cout << "Reviewers ";
                                for (size_t i = 0; i < reviewerSet.size(); i++) {
                                    if (i > 0) cout << ", ";
                                    cout << reviewerSet[i];
                                }
                                cout << " are critical together.\n";
                            }
                        }
                    }
                }
                break;

            case 0:
                cout << "Exiting...\n";
                break;

            default:
                cout << "Invalid option\n";
        }

        if (option != 0) {
            cout << "\nPress ENTER to continue...\n";
            cin.ignore();
            cin.get();
        }

    }
    while(option != 0);
}
