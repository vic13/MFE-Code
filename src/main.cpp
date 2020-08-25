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

    // TTF f1({
    //     make_pair(0,10),make_pair(TTF::period,10)
    // });
    // TTF f2({
    //     make_pair(0,15),make_pair(1,25),make_pair(TTF::period,15)
    // });
    // TTF f1({
    //     make_pair(0,10),make_pair(TTF::period,10)
    // });
    // TTF f2({
    //     make_pair(0,15),make_pair(1,15),make_pair(1,25),make_pair(3,35),make_pair(TTF::period,15)
    // });
    // View::displayTTF({f1,f2});
    // TTF f3 = TTF::chaining(f1,f2);
    // cout << f3.getPoints().size() << endl;
    // View::displayTTF({f1,f2});
    // f3.print();
    // View::displayTTF({f1,f2,f3});
    // return 0;

    OSMGraph osmGraph(PATH_OSM_GRAPHS PARAMS_GRAPH_NAME OSM_GRAPHS_EXTENSION);
    vector<vector<Edge>> adjacencyList = osmGraph.build();
    osmGraph.printImportStats();
    print_graph_properties(adjacencyList);
    #if (PARAMS_VIEW) 
        View::displayNetwork(adjacencyList, osmGraph.getVerticesCoordinates()); 
    #endif

    vector<vector<TDEdge>> adjacencyListTD = convertToTDGraph(adjacencyList);
    TCH tch(adjacencyListTD);
    vector<vector<TCHQueryEdge>> adjacencyListTCH = tch.preprocess();
    print_graph_properties(adjacencyListTCH);
    return 0;

    vector<vector<CHQueryEdge>> adjacencyListCH;
    if (PARAMS_READ_CH_FROM_FILE) {
        adjacencyListCH = readGraphFromFile(PATH_CH_GRAPHS PARAMS_GRAPH_NAME CH_GRAPHS_EXTENSION);
    } else {
        CH ch(adjacencyList);
        adjacencyListCH = ch.preprocess();
        print_graph_properties(adjacencyListCH);
        if (PARAMS_WRITE_CH_TO_FILE) writeGraphToFile(PATH_CH_GRAPHS PARAMS_GRAPH_NAME CH_GRAPHS_EXTENSION, adjacencyListCH);
    }

    if (PARAMS_QUERY_BENCHMARK) Benchmark::queryBenchmark(adjacencyList, adjacencyListCH, PARAMS_NB_RUNS_QUERY_BENCHMARK);
    
    clock.lap(true);
    
}

