#include "../header/maxflow.h"
#include <queue>
#include <climits>

int edmondsKarp(vector<vector<int>> &capacity,vector<vector<int>> &adj,int source,int sink) {
    int v = capacity.size();
    vector<vector<int>> flow(v , vector<int>(v,0));
    int maxFlow = 0;

    while (true) {
        vector<int> parent(v, -1);
        queue<int> q;
        q.push(source);
        parent[source] = source;

        while (!q.empty() && parent[sink] == -1) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                if (parent[v] == -1 && capacity[u][v] - flow[u][v] > 0) {
                    parent[v] = u;
                    q.push(v);
                }
            }
        }
        if (parent[sink] == -1) break;

        int push = INT_MAX;
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            push = min(push, capacity[u][v] - flow[u][v]);
        }

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            flow[u][v] += push;
            flow[v][u] -= push;
        }
        maxFlow += push;
    }
    capacity = flow;
    return maxFlow;
}