#include "Dijkstra.h"
#include <iostream>

using namespace std;

void Dijkstra::findShortestPath(Graph& graph, int source, int destination){
    int size = graph.getSize();

    double* distance = new double[size];

    int* previous = new int[size];

    for (int i = 0; i < size; i++)
    {
        distance[i] = 999999;
        previous[i] = -1;
    }

    distance[source] = 0;

    MinHeap heap(size * 2);

    heap.insert(source, 0);

    while (!heap.isEmpty())
    {
        HeapNode current = heap.extractMin();

        int currentVertex = current.vertex;
        double currentDistance = current.distance;

        if (currentDistance > distance[currentVertex])
        {
            continue;
        }

        vector<Edge> neighbours = graph.getNeighbours(currentVertex);


        for (Edge edge : neighbours)
        {
            if (edge.blocked)
            {
                continue;
            }

            int nextVertex = edge.destination;

            double newDistance = distance[currentVertex] + edge.distance;

            if (newDistance < distance[nextVertex])
            {
                distance[nextVertex] = newDistance;

                previous[nextVertex] = currentVertex;

                heap.insert(nextVertex, newDistance);
            }
        }
    }

    if (distance[destination] == 999999)
    {
        cout << "\nNo route found." << "\n";

        delete[] distance;
        delete[] previous;

        return;
    }

    LinkedList route;

    int current = destination;

    while (current != -1)
    {
        route.insertAtBeginning(current);

        if (current == source)
        {
            break;
        }

        current = previous[current];
    }

    cout << "\n===== SHORTEST ROUTE =====\n";

    cout << "Route: ";

    route.display();

    cout << "Total Distance: " << distance[destination] << "meters" << endl;

    delete[] distance;
    delete[] previous;
}