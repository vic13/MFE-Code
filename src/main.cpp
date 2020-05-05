
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

void dijkstraTest() {
    TestGraph testGraph;
    vector<vector<Edge>> g1 = testGraph.build();

    Dijkstra dijkstra(g1, 0, 1);
    dijkstra.printNodeWeights();
    if (dijkstra.compute()) {
        cout << "Solution found, cost : " << dijkstra.getPathWeight() << endl;
    } else {
        cout << "No solution found" << endl;
    }
    dijkstra.printNodeWeights();
}

int main() {
    cout << endl << "Hello world !" << endl << endl;
    auto t1 = std::chrono::high_resolution_clock::now();

    OSMGraph g;
    vector<vector<Edge>> osmGraph = g.build();

    Dijkstra dijkstra(osmGraph, 123, 155);
    if (dijkstra.compute()) {
        cout << "Solution found, cost : " << dijkstra.getPathWeight() << endl;
    } else {
        cout << "No solution found" << endl;
    }
    dijkstra.printNodeWeights();

    //dijkstraTest();

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout << "Duration : " << duration/1000000.0 << endl;
    
}

