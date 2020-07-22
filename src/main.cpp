
#include <vector>
#include <iostream>
#include <chrono>

using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::make_pair;

#include "Edge.hpp"
#include "utils.hpp"
#include "Dijkstra.hpp"
#include "TestGraph.hpp"
#include "OSMGraph.hpp"
#include "BarabasiAlbertGraph.hpp"
#include "ErdosRenye.hpp"
#include "View.hpp"
#include "CH.hpp"


void performRandomDijkstra(vector<vector<Edge>> adjacencyList, vector<pair<float, float>> verticesCoordinates) {
    int s = randomInt(adjacencyList.size());
    int t = randomInt(adjacencyList.size());
    cout << "Search from vertex " << s << " to vertex " << t << endl;
    Dijkstra dijkstra(adjacencyList, s, t);
    if (dijkstra.compute()) {
        cout << "Solution found, cost : " << dijkstra.getPathWeight() << endl;
        cout << "Path : ";
        vector<int> path = dijkstra.getPath();
        print_vector(path);
        View::display(adjacencyList, path, verticesCoordinates, s, t);
    } else {
        cout << "No solution found" << endl;
        View::display(adjacencyList, vector<int>(), verticesCoordinates, s, t);
    }
}

int main() {
    cout << endl << "Hello world !" << endl << endl;

    initRandom();

    auto t1 = std::chrono::high_resolution_clock::now();

    OSMGraph osmGraph;
    vector<vector<Edge>> adjacencyList = osmGraph.build();

    CH ch(adjacencyList);
    ch.constructCH();
    // osmGraph.printImportStats();
    // BarabasiAlbertGraph barabasiAlbertGraph(10000, 4);
    // vector<vector<Edge>> adjacencyList = barabasiAlbertGraph.build();
    // ErdosRenye erdosRenyeGraph(10000, 9000);
    // vector<vector<Edge>> adjacencyList = erdosRenyeGraph.build();

    // performRandomDijkstra(adjacencyList, osmGraph.getVerticesCoordinates());
    
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout << "Program duration : " << duration/1000000.0 << " seconds" << endl;
    
}

