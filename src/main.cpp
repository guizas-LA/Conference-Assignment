/**
 * @file main.cpp
 * @brief Entry point for the Scientific Conference Review Assignment Tool.
 * @author Conference Organization System
 * 
 * This program provides a tool to assist in assigning paper reviews to qualified
 * reviewers based on their expertise and topic availability. It supports both
 * interactive menu mode and batch mode processing.
 */

#include <iostream>
#include <string>
#include "../header/batch.h"
#include "../header/menu.h"
using namespace std;


int main(int argc, char* argv[]) {
    if (argc == 4 && string(argv[1]) == "-b") {
        batchMode(argv[2], argv[3]);
    }
    else {
        menu();
    }

    return 0;
}