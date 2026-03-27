#include "../header/show.h"
#include <iostream>
using namespace std;

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
