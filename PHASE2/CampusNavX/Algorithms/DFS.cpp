#include "DFS.h"
#include <iostream>

using namespace std;

void DFS::dfs(Graph &graph, int current, bool visited[]){
    visited[current] = true;

    cout << current << " ";

    vector<Edge> neighbours = graph.getNeighbours(current);

    for(Edge edge : neighbours){
        if (!visited[edge.destination] && !edge.blocked){
            dfs(graph, edge.destination, visited);
        }
    }
}

void DFS::traverse(Graph &graph, int start){
    int size = graph.getSize();

    bool *visited = new bool[size];

    for (int i = 0; i < size; i++){
        visited[i] = false;
    }

    cout << "DFS Traversal:";

    dfs(graph, start, visited);
    cout << endl;
    delete[] visited;
}