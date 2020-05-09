
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
#include "View.hpp"


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
        View::display(adjacencyList, path, osmGraph.getVerticesCoordinates());
    } else {
        cout << "No solution found" << endl;
    }
    
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout << "Duration : " << duration/1000000.0 << endl;
    
}

