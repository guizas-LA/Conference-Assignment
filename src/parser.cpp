#include "../header/parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;


string trim(string s) {
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    s.erase(s.find_last_not_of(" \t\r\n") + 1);
    return s;
}

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

bool readFile(const string &filename,vector<Submission> &submissions,vector<Reviewer> &reviewers,map<string, string> &parameters) {
    ifstream file("../data/" + filename + ".csv");

    if (!file.is_open()) {
        cerr << "Error opening file.\n";
        return false;
    }

    submissions.clear();
    reviewers.clear();
    parameters.clear();

    string line;
    string section = "";

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

                reviewers.push_back(r);
            }
            else if (section == "parameters") {

                if (parts.size() >= 2) {
                    parameters[parts[0]] = parts[1];
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
    return true;
}