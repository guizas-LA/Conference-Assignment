#ifndef PROJECT_BATCH_H
#define PROJECT_BATCH_H
#include <string>
using namespace std;

/**
 * @brief Executes the program in batch mode.
 *
 * The batch interface reads a dataset file and writes all requested output
 * sections to a single output file.
 *
 * @param inputFile Dataset file name or path.
 * @param riskFile Output file name or path.
 */
void batchMode(string inputFile, string riskFile);


#endif //PROJECT_BATCH_H
