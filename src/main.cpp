
#include <vector>
#include <iostream>
#include <chrono>

using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::make_pair;
using std::string;

#include "parameters.hpp"
#include "TTF.hpp"
#include "Edge.hpp"
#include "utils.hpp"
#include "Dijkstra.hpp"
#include "TestGraph.hpp"
#include "OSMGraph.hpp"
#include "BarabasiAlbertGraph.hpp"
#include "ErdosRenye.hpp"
#if (PARAMS_VIEW)
    #include "View.hpp"
#endif
#include "CH.hpp"
#include "DijkstraCHQuery.hpp"
#include "Benchmark.hpp"

int main() {
    cout << endl << "Hello world !" << endl << endl;
    initRandom();
    Clock clock("Program duration", false);
    
    // TTF f1({make_pair(0,5), make_pair(10,0)});
    // TTF f2({make_pair(0,0), make_pair(7,0), make_pair(7,5), make_pair(10,5)});
    TTF f1 = randomTTF();
    TTF f2 = randomTTF();
    // cout << f1.evaluate(400);
    // View::displayTTF({f1, f2, TTF::minimum(f1,f2)});
    View::displayTTF({f1, f2, TTF::chaining(f1,f2)});

    return 0;

    OSMGraph osmGraph(PATH_OSM_GRAPHS PARAMS_GRAPH_NAME OSM_GRAPHS_EXTENSION);
    vector<vector<Edge>> adjacencyList = osmGraph.build();
    osmGraph.printImportStats();
    print_graph_properties(adjacencyList);
    #if (PARAMS_VIEW) 
        View::displayNetwork(adjacencyList, osmGraph.getVerticesCoordinates()); 
    #endif

    vector<vector<CHQueryEdge>> adjacencyListCH;
    if (PARAMS_READ_CH_FROM_FILE) {
        adjacencyListCH = readGraphFromFile(PATH_CH_GRAPHS PARAMS_GRAPH_NAME CH_GRAPHS_EXTENSION);
    } else {
        CH ch(adjacencyList);
        adjacencyListCH = ch.preprocess();
        if (PARAMS_WRITE_CH_TO_FILE) writeGraphToFile(PATH_CH_GRAPHS PARAMS_GRAPH_NAME CH_GRAPHS_EXTENSION, adjacencyListCH);
    }

    if (PARAMS_QUERY_BENCHMARK) Benchmark::queryBenchmark(adjacencyList, adjacencyListCH, PARAMS_NB_RUNS_QUERY_BENCHMARK);
    
    clock.lap(true);
    
}

