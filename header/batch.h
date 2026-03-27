/**
 * @file batch.h
 * @brief Batch mode execution for automated processing.
 * @author Conference Organization System
 */

#ifndef PROJECT_BATCH_H
#define PROJECT_BATCH_H
#include <string>
using namespace std;

/**
 * @brief Executes the review assignment and risk analysis in batch mode.
 * 
 * Reads configuration and data from an input file, performs review assignment
 * generation and risk analysis, then writes results to an output file.
 * Suitable for automated processing via command-line scripts.
 * 
 * @param inputFile Path to the input CSV file containing submissions, reviewers, and parameters.
 * @param riskFile Path to the output file where results (assignments and risk analysis) will be written.
 * 
 * @see analyzeReviewerRisk()
 * @see writeRiskAnalysis()
 */
void batchMode(string inputFile, string riskFile);


#endif //PROJECT_BATCH_H