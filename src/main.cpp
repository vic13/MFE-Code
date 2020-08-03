
#include <vector>
#include <iostream>
#include <chrono>

using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::make_pair;
using std::string;

#include "Parameters.hpp"
#include "Edge.hpp"
#include "utils.hpp"
#include "Dijkstra.hpp"
#include "TestGraph.hpp"
#include "OSMGraph.hpp"
#include "BarabasiAlbertGraph.hpp"
#include "ErdosRenye.hpp"
#include "View.hpp"
#include "CH.hpp"
#include "DijkstraCHQuery.hpp"

bool testCorrectness(vector<vector<Edge>> adjacencyList, vector<vector<CHQueryEdge>> adjacencyListCH, int nbRuns = 100) {
    bool correct = true;
    float eps = 0.002;
    
    print_graph_properties(adjacencyList);
    print_graph_properties(adjacencyListCH);

    auto avgDijkstra = 0.0;
    auto avgDijkstraCH = 0.0;
    int avgSearchSpaceDijkstra = 0;
    int avgSearchSpaceDijkstraCH = 0;

    int wrong = 0;

    for (int i = 0; i<nbRuns; i++) {
        int s = randomInt(adjacencyList.size());
        int t = randomInt(adjacencyList.size());

        Dijkstra dijkstra(adjacencyList, s, t);
        DijkstraCHQuery dijkstraCH(adjacencyListCH, s, t);
        
        auto t1 = std::chrono::high_resolution_clock::now();
        bool c1 = dijkstra.compute();
        auto t2 = std::chrono::high_resolution_clock::now();
        bool c2 = dijkstraCH.compute();
        auto t3 = std::chrono::high_resolution_clock::now();

        avgDijkstra += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        avgDijkstraCH += std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
        avgSearchSpaceDijkstra += dijkstra.getSearchSpace();
        avgSearchSpaceDijkstraCH += dijkstraCH.getSearchSpace();

        if (c1 != c2) {
            cout << "Wrong for s : " << s << " and t : " << t << " (c1 : " << c1 << " and c2 : " << c2 << endl;
            correct = false;
        } else {
            if (c1 == true) {
                float error = abs(dijkstra.getPathWeight() - dijkstraCH.getPathWeight());
                if (error > eps) {
                    cout << "Wrong for s : " << s << " and t : " << t << " (w1 : " << dijkstra.getPathWeight() << " and w2 : " << dijkstraCH.getPathWeight() << " dw : " << dijkstraCH.getPathWeight()-dijkstra.getPathWeight() << endl;
                    correct = false;
                }
            }
        }
    }
    std::cout << "Average time dijkstra : " << avgDijkstra/(1000.0*nbRuns) << " ms" << endl;
    std::cout << "Average time dijkstraCH : " << avgDijkstraCH/(1000.0*nbRuns) << " ms" << endl;
    std::cout << "Speed-up : " << avgDijkstra/avgDijkstraCH << endl;
    std::cout << "Average search space dijkstra : " << (float)avgSearchSpaceDijkstra/(nbRuns) << " settled nodes" << endl;
    std::cout << "Average search space dijkstraCH : " << (float)avgSearchSpaceDijkstraCH/(nbRuns) << " settled nodes" << endl;
    return correct;
}

vector<int> createLayeredGraph(vector<vector<Edge>>& graph, int nbLayers, int nbTransitionVertices) {
    int nbBaseVertices = graph.size();
    // Define transition vertices
    vector<int> transitionVertices;
    for (int i=0; i<nbTransitionVertices; i++) {
        transitionVertices.push_back(i * nbBaseVertices / nbTransitionVertices);
    }
    print_vector(transitionVertices);
    // Layers
    for (int layer=1; layer<nbLayers; layer++) {
        // Create layer graph
        int offset = nbBaseVertices*layer;
        for (int baseVertex=0; baseVertex<nbBaseVertices; baseVertex++) {
            vector<Edge> layerEdges;
            vector<Edge> baseEdges = graph[baseVertex];
            for (auto& edge : baseEdges) {
                layerEdges.push_back(Edge(edge.getDestinationVertex()+offset, edge.getWeight()));
            }
            graph.push_back(layerEdges);
        }
    }
    // Connect to base layer
    vector<int> verticesOrder = vector<int>(graph.size(), 0);
    for (int layer=1; layer<nbLayers; layer++) {
        int offset = nbBaseVertices*layer;
        for (int transitionVertex : transitionVertices) {
            graph[transitionVertex].push_back(Edge(transitionVertex+offset, 100)); // base to up layer
            graph[transitionVertex+offset].push_back(Edge(transitionVertex, 0)); // up layer to base
            verticesOrder[transitionVertex] = 2;
            verticesOrder[transitionVertex+offset] = 1;
        }
    }
    return verticesOrder;
}


int main() {
    cout << endl << "Hello world !" << endl << endl;

    initRandom();

    auto t1 = std::chrono::high_resolution_clock::now();

    OSMGraph osmGraph("./OSM_graph_data/graphBxlCenter.json");
    vector<vector<Edge>> adjacencyList = osmGraph.build();
    print_graph_properties(adjacencyList);

    vector<int> transitionVertices = createLayeredGraph(adjacencyList, 5, 10);
    //osmGraph.printImportStats();

    CH ch(adjacencyList, transitionVertices);
    vector<vector<CHQueryEdge>> adjacencyListCH = ch.preprocess();
    
    // writeGraphToFile("./OSM_graph_serialized/graph", adjacencyListCH);
    // vector<vector<CHQueryEdge>> adjacencyListCH = readGraphFromFile("./OSM_graph_serialized/graphBxlCenter");
    // print_graph_properties(adjacencyListCH2);
    // printNetwork(adjacencyListCH, 2);
    // printNetwork(adjacencyListCH2, 2);
    
    // BarabasiAlbertGraph barabasiAlbertGraph(2000, 2);
    // vector<vector<Edge>> adjacencyList = barabasiAlbertGraph.build();
    // ErdosRenye erdosRenyeGraph(10000, 9000);
    // vector<vector<Edge>> adjacencyList = erdosRenyeGraph.build();

    testCorrectness(adjacencyList, adjacencyListCH) ? cout << "Correct" << endl : cout << "Not correct" << endl;
    
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout << "Program duration : " << duration/1000000.0 << " seconds" << endl;
    
}

