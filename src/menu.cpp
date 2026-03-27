#include "../header/menu.h"
#include "../header/parser.h"
#include "../header/structures.h"
#include <iostream>
#include "../header/assignment.h"
#include "../header/show.h"
using namespace std;

/**
 * @brief Displays the main menu and handles user interaction.
 * 
 * This function provides a console-based interface for the Scientific
 * Conference Tool. It allows the user to:
 * - Load a dataset from a file
 * - View submissions, reviewers, and parameters
 * - Generate reviewer assignments
 * - Perform (future) risk analysis
 * - Exit the application
 * 
 * The function continuously prompts the user for input until the exit
 * option (0) is selected.
 * 
 * Internally, it maintains collections of:
 * - Submissions
 * - Reviewers
 * - Parameters
 * 
 * It also ensures that required data is loaded before performing
 * operations like assignment generation or analysis.
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

            /**
             * @brief Loads dataset from a file.
             * 
             * Prompts the user for a filename and attempts to read
             * submissions, reviewers, and parameters using readFile().
             * Displays summary information upon success.
             */
            case 1:
                cout << "File name: ";
                cin >> filename;
                if (readFile(filename, submissions, reviewers, parameters)) {
                    cout << "\nFile loaded successfully!\n";
                    cout << "Submissions: " << submissions.size() << endl;
                    cout << "Reviewers: " << reviewers.size() << endl;
                    cout << "Parameters: " << parameters.size() << endl;
                }
                break;

            /**
             * @brief Displays all loaded submissions.
             */
            case 2:
                showSubmissions(submissions);
                break;

            /**
             * @brief Displays all loaded reviewers.
             */
            case 3:
                showReviewers(reviewers);
                break;

            /**
             * @brief Displays all loaded parameters.
             */
            case 4:
                showParameters(parameters);
                break;
                /**
            * @brief Generates reviewer assignments for submissions and writes them to a file.
            *
            * This case handles the creation of assignment mappings between submissions
            * and reviewers. It first checks whether the required datasets (submissions
            * and reviewers) are loaded. If not, it notifies the user.
            *
            * If the datasets are available:
            * - Initializes a vector of Assignment objects.
            * - Calls primaryAssignments() to generate the assignment mappings based on
            *   the given parameters.
            * - Determines the output file name:
            *   - Uses the "OutputFileName" parameter if provided.
            *   - Defaults to "output.csv" otherwise.
            * - Writes the generated assignments to the specified file using writeAssignments().
            *
            * @note Requires submissions and reviewers to be non-empty.
            *
            * @warning If no dataset is loaded, assignment generation will not proceed.
            *
            * @see primaryAssignments()
            * @see writeAssignments()
            */
            
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

            /**
             * @brief Runs risk analysis (not yet implemented).
             * 
             * Placeholder for future functionality.
             */
            case 6:
                cout << "\nRunning risk analysis...\n";

                if (submissions.empty() || reviewers.empty()) {
                    cout << "Load a dataset first!\n";
                }
                else {
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
                break;

            /**
             * @brief Exits the application.
             */
            case 0:
                cout << "Exiting...\n";
                break;

            /**
             * @brief Handles invalid menu options.
             */
            default:
                cout << "Invalid option\n";
        }

        /**
         * @brief Pauses execution until user presses ENTER.
         */
        if (option != 0) {
            cout << "\nPress ENTER to continue...\n";
            cin.ignore();
            cin.get();
        }

    }
    while(option != 0);
}
