
#include <vector>
#include <iostream>
#include <chrono>

using std::cout;
using std::endl;
using std::vector;

#include "Edge.hpp"
#include "Dijkstra.hpp"
#include "BarabasiAlbert.hpp"




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
    cout << "hello world" << endl;
    auto t1 = std::chrono::high_resolution_clock::now();

    // Parameters
    int n = 100;

    // Build network
    BarabasiAlbert graph(n);
    vector<vector<Edge>> graphAsNeighbours = graph.build(); // Get the network as a list of n lists of neighbours
    auto t2 = std::chrono::high_resolution_clock::now();
    auto buildDuration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout << "Build duration : " << buildDuration/1000000.0 << endl;

    //printNetwork();
    vector<vector<Edge>> simpleGraph(6, vector<Edge>());
    simpleGraph[0].push_back(Edge(1,7));
    simpleGraph[0].push_back(Edge(2,2));
    simpleGraph[1].push_back(Edge(0,7));
    simpleGraph[1].push_back(Edge(2,3));
    simpleGraph[1].push_back(Edge(3,4));
    simpleGraph[2].push_back(Edge(0,2));
    simpleGraph[2].push_back(Edge(1,3));
    simpleGraph[2].push_back(Edge(3,4));
    simpleGraph[2].push_back(Edge(4,1));
    simpleGraph[3].push_back(Edge(1,4));
    simpleGraph[3].push_back(Edge(2,4));
    simpleGraph[3].push_back(Edge(5,5));
    simpleGraph[4].push_back(Edge(2,1));
    simpleGraph[4].push_back(Edge(5,3));
    simpleGraph[5].push_back(Edge(3,5));
    simpleGraph[5].push_back(Edge(4,3));

    Dijkstra dijkstra(simpleGraph, 0, 1);
    dijkstra.printNodeWeights();
    if (dijkstra.compute()) {
        cout << "Solution found, cost : " << dijkstra.getPathWeight() << endl;
    } else {
        cout << "No solution found" << endl;
    }
    dijkstra.printNodeWeights();
    
}