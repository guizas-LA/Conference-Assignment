#include "../header/show.h"
#include <iostream>
using namespace std;

/**
 * @brief Displays all submissions.
 * 
 * This function prints a formatted list of all submissions,
 * including their ID, title, authors, email, and assigned
 * primary and secondary reviewers.
 * 
 * If no submissions are available, a message is displayed.
 * 
 * @param subs Vector containing all submissions.
 */
void showSubmissions(const vector<Submission> &subs) {
    cout << "=======================================\n";
    cout << "|              Submissions            |\n";
    cout << "=======================================\n";

    if (subs.empty()) {
        cout << "No submissions loaded.\n";
        return;
    }

    for (const auto &s : subs) {
        cout << "ID: " << s.id
             << " | Title: " << s.title
             << " | Authors: " << s.authors
             << " | Email: " << s.email
             << " | Primary: " << s.primary
             << " | Secondary: " << s.secondary
             << endl;
    }
}

/**
 * @brief Displays all reviewers.
 * 
 * This function prints a formatted list of all reviewers,
 * including their ID, name, email, and primary and secondary
 * expertise domains.
 * 
 * If no reviewers are available, a message is displayed.
 * 
 * @param revs Vector containing all reviewers.
 */
void showReviewers(const vector<Reviewer> &revs) {
    cout << "=======================================\n";
    cout << "|               Reviewers             |\n";
    cout << "=======================================\n";

    if (revs.empty()) {
        cout << "No reviewers loaded.\n";
        return;
    }

    for (const auto &r : revs) {
        cout << "ID: " << r.id
             << " | Name: " << r.name
             << " | Email: " << r.email
             << " | Primary: " << r.primary
             << " | Secondary: " << r.secondary
             << endl;
    }
}

/**
 * @brief Displays all configuration parameters.
 * 
 * This function prints all key-value pairs representing
 * configuration parameters used in the system.
 * 
 * If no parameters are available, a message is displayed.
 * 
 * @param params Map containing parameter names and values.
 */
void showParameters(const map<string,string> &params) {
    cout << "=======================================\n";
    cout << "|              Parameters             |\n";
    cout << "=======================================\n";

    if (params.empty()) {
        cout << "No parameters loaded.\n";
        return;
    }

    for (const auto &p : params) {
        cout << p.first << " = " << p.second << endl;
    }
}