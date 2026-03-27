/**
 * @file maxflow.h
 * @brief Maximum flow algorithm implementation.
 * @author Conference Organization System
 */

#ifndef PROJECT_MAXFLOW_H
#define PROJECT_MAXFLOW_H

#include <vector>

using namespace std;

/**
 * @brief Computes maximum flow using Edmonds-Karp algorithm (BFS-based Ford-Fulkerson).
 * 
 * Finds the maximum flow in a flow network from source to sink using
 * the Edmonds-Karp algorithm, which improves Ford-Fulkerson by using
 * BFS to find augmenting paths instead of DFS.
 * 
 * @param capacity Reference to flow capacity matrix. Modified in-place to store the flow.
 * @param adj Reference to adjacency list. Represents the network structure.
 * @param source Source node index.
 * @param sink Sink node index.
 * @return Maximum flow value from source to sink.
 * 
 * @note Time Complexity: O(V*E^2) where V is number of vertices and E is number of edges.
 *       Space Complexity: O(V^2) for the capacity and flow matrices.
 */
int edmondsKarp(vector<vector<int>> &capacity,vector<vector<int>> &adj,int source,int sink);

#endif //PROJECT_MAXFLOW_H