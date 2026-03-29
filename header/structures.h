#ifndef PROJECT_STRUCTURES_H
#define PROJECT_STRUCTURES_H
#include <string>
using namespace std;

/**
 * @struct Submission
 * @brief Stores the relevant information of a paper submission.
 */
struct Submission {
    int id;
    string title;
    string authors;
    string email;
    int primary;
    int secondary;
};

/**
 * @struct Reviewer
 * @brief Stores the relevant information of a reviewer and their expertise domains.
 */
struct Reviewer {
    int id;
    string name;
    string email;
    int primary;
    int secondary;
};
#endif //PROJECT_STRUCTURES_H
