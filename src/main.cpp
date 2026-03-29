#include <iostream>
#include <string>
#include "../header/batch.h"
#include "../header/menu.h"
using namespace std;

/**
 * @brief Entry point of the application.
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument values.
 * @return Exit status code.
 */
int main(int argc, char* argv[]) {
    if (argc == 4 && string(argv[1]) == "-b") {
        batchMode(argv[2], argv[3]);
    }
    else {
        menu();
    }

    return 0;
}
