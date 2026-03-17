#include "../header/batch.h"
#include "../header/parser.h"
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

    ofstream out(riskFile);

    if (!out.is_open()) {
        cerr << "Error creating output file.\n";
        return;
    }

    out << "#Batch executed\n";
    out << "Submissions: " << subs.size() << "\n";
    out << "Reviewers: " << revs.size() << "\n";

    out.close();

    cout << "Batch completed.\n";
}