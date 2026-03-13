#include "../header/menu.h"
#include <iostream>
using namespace std;

void menu() {

    int option;

    do {
        cout << "=======================================\n";
        cout << "|     Scientific Conference Tool      |\n";
        cout << "=======================================\n";
        cout << "| 1 - Load dataset                    |\n";
        cout << "| 2 - List submissions                |\n";
        cout << "| 3 - List reviewers                  |\n";
        cout << "| 4 - Generate assignments            |\n";
        cout << "| 5 - Risk analysis                   |\n";
        cout << "| 0 - Exit                            |\n";
        cout << "=======================================\n";
        cout << "Option: ";

        cin >> option;

        switch(option) {

            case 1:
                cout << "Loading dataset...\n";
                break;

            case 2:
                cout << "Listing submissions...\n";
                break;

            case 3:
                cout << "Listing reviewers...\n";
                break;

            case 4:
                cout << "Generating assignments...\n";
                break;

            case 5:
                cout << "Running risk analysis...\n";
                break;

            case 0:
                cout << "Exiting...\n";
                break;

            default:
                cout << "Invalid option\n";
        }

    } while(option != 0);
}