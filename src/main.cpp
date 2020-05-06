
#include <vector>
#include <iostream>
#include <chrono>

using std::cout;
using std::endl;
using std::vector;

#include "Edge.hpp"
#include "Dijkstra.hpp"
#include "BarabasiAlbert.hpp"
#include "TestGraph.hpp"
#include "OSMGraph.hpp"
#include "View.hpp"



void printNetwork(vector<vector<Edge>> graph) {
    for (int a=0; a<100; a++) {
        vector<Edge> neighbours = graph[a];
        cout << "nb neighbours : " << neighbours.size() << endl;
        for (int b=0; b<neighbours.size(); b++) {
            cout << "weight : " << neighbours[b].getWeight() << endl;
            cout << "dest : " << neighbours[b].getDestinationNode() << endl;
        }
    }
}

int main() {
    cout << endl << "Hello world !" << endl << endl;
    auto t1 = std::chrono::high_resolution_clock::now();

    OSMGraph osmGraph;
    vector<vector<Edge>> adjacencyList = osmGraph.build();

    Dijkstra dijkstra(adjacencyList, 100, 10);
    if (dijkstra.compute()) {
        cout << "Solution found, cost : " << dijkstra.getPathWeight() << endl;
        cout << "Path : ";
        vector<int> path = dijkstra.getPath();
        print_vector(path);
        View::display(adjacencyList, path, osmGraph.getNodesCoordinates());
    } else {
        cout << "No solution found" << endl;
    }
    //dijkstra.printNodeWeights();

    
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout << "Duration : " << duration/1000000.0 << endl;
    
}

