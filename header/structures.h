/**
 * @file structures.h
 * @brief Core data structures for submissions and reviewers.
 * @author Conference Organization System
 */

#ifndef PROJECT_STRUCTURES_H
#define PROJECT_STRUCTURES_H
#include <string>
using namespace std;

/**
 * @struct Submission
 * @brief Represents a paper submission to the conference.
 * 
 * Contains metadata about a submitted paper including identification,
 * authorship information, and topic domains.
 */
struct Submission {
    int id;              /**< Unique identifier for the submission */
    string title;        /**< Title of the paper */
    string authors;      /**< Names of the paper authors */
    string email;        /**< Contact email address */
    int primary;         /**< Primary topic domain ID */
    int secondary;       /**< Secondary topic domain ID (-1 if not specified) */
};

/**
 * @struct Reviewer
 * @brief Represents a reviewer for conference submissions.
 * 
 * Contains information about a reviewer including identification,
 * contact details, and areas of expertise.
 */
struct Reviewer {
    int id;              /**< Unique identifier for the reviewer */
    string name;         /**< Name of the reviewer */
    string email;        /**< Contact email address */
    int primary;         /**< Primary expertise domain ID */
    int secondary;       /**< Secondary expertise domain ID (-1 if not specified) */
};
#endif //PROJECT_STRUCTURES_H