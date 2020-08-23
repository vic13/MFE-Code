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
#include "Graph_Generation/OSMGraph.hpp"
#include "Graph_Generation/BarabasiAlbertGraph.hpp"
#include "Graph_Generation/ErdosRenye.hpp"
#if (PARAMS_VIEW)
    #include "View.hpp"
#endif
#include "CH/CHGraph.hpp"
#include "CH/CHTemplate.hpp"
#include "CH/CH.hpp"
#include "CH/TCH.hpp"
#include "CH/DijkstraCHQuery.hpp"
#include "Benchmark.hpp"

int main() {
    cout << endl << "Hello world !" << endl << endl;
    initRandom();
    Clock clock("Program duration", false);
    
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

