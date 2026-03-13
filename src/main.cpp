#include <iostream>
#include <string>
#include "../header/batch.h"
#include "../header/menu.h"
using namespace std;


int main(int argc, char* argv[]) {

    if (argc == 4 && string(argv[1]) == "-b") {
        string inputFile = argv[2];
        string riskFile = argv[3];
        batchMode(inputFile, riskFile);
    }

    else {
        menu();
    }

    return 0;
}