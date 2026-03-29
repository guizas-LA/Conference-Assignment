#ifndef PROJECT_MAXFLOW_H
#define PROJECT_MAXFLOW_H

#include <vector>

using namespace std;

/**
 * @brief Computes a maximum flow using the Edmonds-Karp algorithm.
 *
 * The function updates the given residual capacity matrix with the flow found
 * and returns the total flow value between the source and sink nodes.
 *
 * @param capacity Residual capacity matrix of the network.
 * @param adj Adjacency list of the network.
 * @param source Source node index.
 * @param sink Sink node index.
 * @return The maximum flow value.
 *
 * @note Time complexity: O(V * E^2).
 */
int edmondsKarp(vector<vector<int>> &capacity,vector<vector<int>> &adj,int source,int sink);

#endif //PROJECT_MAXFLOW_H
