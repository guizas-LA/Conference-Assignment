#include "../header/menu.h"
#include "../header/parser.h"
#include "../header/structures.h"
#include <iostream>
#include "../header/show.h"
using namespace std;


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
                cout << "File name: ";
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
                    cout << "(Not implemented yet)\n";
                }
                break;

            case 6:
                cout << "\nRunning risk analysis...\n";

                if (submissions.empty() || reviewers.empty()) {
                    cout << "Load a dataset first!\n";
                }
                else {
                    cout << "(Not implemented yet)\n";
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