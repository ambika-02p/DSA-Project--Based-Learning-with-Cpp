#include "BFS.h"
#include <iostream>

using namespace std;

void BFS::traverse(Graph &graph, int start){
    int size = graph.getSize();
    bool *visited = new bool[size];

    for (int i = 0; i < size; i++){
        visited[i] = false;
    }

    Queue q(size);

    visited[start] = true;

    q.enqueue(start);

    cout << "BFS Traversal:";

    while (!q.isEmpty()){
        int current = q.dequeue();
        cout << current << " ";

        vector<Edge> neighbours = graph.getNeighbours(current);

        for (Edge edge : neighbours){
            int next = edge.destination;

            if (!visited[next] && !edge.blocked){
                visited[next] = true;
                q.enqueue(next);
            }
        }
    }

    cout << endl;
    delete[] visited;
}