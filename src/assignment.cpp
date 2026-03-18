#include "../header/assignment.h"
#include <iostream>
#include <fstream>

using namespace std;

void primaryAssignments(vector<Submission> &subs , vector<Reviewer> &revs , const map<string,string> &params, vector<MissingReviews> &missing){
    int minReviews = 1;
    int maxReviews = 1;

    auto itMin = params.find("MinReviewsPerSubmission");
    if (itMin != params.end()) minReviews = stoi(itMin->second);

    auto itMax = params.find("MaxReviewsPerReviewer");
    if (itMax != params.end()) maxReviews = stoi(itMax->second);

    map<int,int> reviewerLoad;

    for (const auto &r : revs) reviewerLoad[r.id] = 0;

    for (auto &s : subs) {
        vector<int> assigned;

        for (auto &r : revs) {
            if (r.primary == s.primary && reviewerLoad[r.id] < maxReviews) {
                assigned.push_back(r.id);
                reviewerLoad[r.id]++;
                if (assigned.size() >= minReviews) break;
            }
        }
        if (!assigned.empty()) {
            s.primary = assigned[0];
            if (assigned.size() > 1) {
                s.secondary = assigned[1];
            }
            else {
                s.secondary = -1;
            }
        }
        else {
            s.primary = -1;
            s.secondary = -1;
        }
        if (assigned.size() < minReviews) {
            missing.push_back({s.id, s.primary, minReviews - (int)assigned.size()});
        }
    }
    cout << "Assignments generated (Primary Domain only).\n";
}

void writeAssignments(const vector<Submission> &subs , const vector<Reviewer> &revs , const string &outputFileName, const vector<MissingReviews> &missing) {
    ofstream out(outputFileName);
    if (!out.is_open()) {
        cerr << "Error opening output file: " << outputFileName << endl;
        return;
    }
    int total = 0;

    out << "#SubmissionId,ReviewerId,Match\n";
    for (const auto &s : subs) {
        if (s.primary != -1) {
            out << s.id << "," << s.primary << "," << s.primary << "\n"; total++;
        }
        if (s.secondary != -1) {
            out << s.id << "," << s.secondary << "," << s.primary << "\n"; total++;
        }
    }

    out << "#ReviewerId,SubmissionId,Match\n";
    for (const auto &r : revs) {
        for (const auto &s : subs) {
            if (s.primary == r.id || s.secondary == r.id) {
                out << r.id << "," << s.id << "," << r.primary << "\n";
            }
        }
    }

    out << "#Total: " << total << "\n";

    if (!missing.empty()) {
        out << "#MissingReviews\n#SubmissionId,Domain,Missing\n";
        for (const auto &m : missing) {
            out << m.submissionId << "," << m.domain << "," << m.missing << "\n";
        }
    }
    out.close();
    cout << "Assignments written to " << outputFileName << endl;

    if (!missing.empty()) {
        cout << "Warning: Some submissions did not reach the minimum number of reviews!\n";
        for (const auto &m : missing) {
            cout << "Submission " << m.submissionId << " missing " << m.missing << " reviews.\n";
        }
    }
}
