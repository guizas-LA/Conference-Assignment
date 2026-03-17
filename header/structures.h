#ifndef PROJECT_STRUCTURES_H
#define PROJECT_STRUCTURES_H
#include <string>
using namespace std;

struct Submission {
    int id;
    string title;
    string authors;
    string email;
    int primary;
    int secondary;
};

struct Reviewer {
    int id;
    string name;
    string email;
    int primary;
    int secondary;
};
#endif //PROJECT_STRUCTURES_H