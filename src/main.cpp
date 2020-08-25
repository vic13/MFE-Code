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
#include "Utils/Random.hpp"
#include "Utils/Clock.hpp"
#include "TTF.hpp"
#include "Edge.hpp"
#include "Utils/GraphUtils.hpp"
#include "Utils/IO.hpp"
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
    Random::initRandom();
    Clock clock("Program duration", false);

    float r = 10;
    // TTF f1({
    //     make_pair(0,5*10*r),make_pair(100,5*9*r),make_pair(101,5*9.5*r),make_pair(200,5*8*r),make_pair(TTF::period,5*9*r)
    // });
    // TTF f2({
    //     make_pair(0,15*r),make_pair(30,25*r),make_pair(TTF::period,10*r)
    // });
    // TTF f1({
    //     make_pair(0,10),make_pair(TTF::period,10)
    // });
    // TTF f2({
    //     make_pair(0,15),make_pair(100,15),make_pair(100,25),make_pair(300,35),make_pair(TTF::period,15)
    // });
    // View::displayTTF({f1,f2});
    // TTF f3 = TTF::chaining(f1,f2);
    // cout << f3.getPoints().size() << endl;
    // View::displayTTF({f1,f2});
    // f3.print();
    // View::displayTTF({f1,f2,f3});
    // TTF f1 = TTF::randomTTF();
    // TTF f2 = TTF::randomTTF();
    // View::displayTTF({f1,f2});
    // Clock clock2("chaining2", false);
    // TTF f4 = TTF::chaining2(f1,f2);
    // f4.print();
    // clock2.lap(true);

    // Clock clock1("chaining1", false);
    // TTF f3 = TTF::chaining(f1,f2);
    // f3.print();
    // clock1.lap(true);
    
    // View::displayTTF({f1,f2,f3});
    // TTF f0(3);
    // for (int i=0; i<1000; i++) {
    //     cout << i << " : " << f0.getPoints().size() << endl;
    //     TTF f(Random::random01()*10);
    //     f0 = TTF::chaining(f0, f);
    //     if (!f0.respectsFIFO()) {
    //         cout << i << endl;
    //         cout <<std::setprecision(9)<< f0.getPoints().front().second << endl;
    //         cout <<std::setprecision(9)<< f0.getPoints().back().second << endl;
    //         break;
    //     }
    // }
    // View::displayTTF({f0});
    // return 0;

    OSMGraph osmGraph(PATH_OSM_GRAPHS PARAMS_GRAPH_NAME OSM_GRAPHS_EXTENSION);
    vector<vector<Edge>> adjacencyList = osmGraph.build();
    osmGraph.printImportStats();
    GraphUtils::printGraphProperties(adjacencyList);
    #if (PARAMS_VIEW) 
        View::displayNetwork(adjacencyList, osmGraph.getVerticesCoordinates());
    #endif

    vector<vector<TDEdge>> adjacencyListTD = GraphUtils::convertToTDGraph(adjacencyList);
    TCH tch(adjacencyListTD);
    vector<vector<TCHQueryEdge>> adjacencyListTCH = tch.preprocess();
    GraphUtils::printGraphProperties(adjacencyListTCH);
    return 0;

    vector<vector<CHQueryEdge>> adjacencyListCH;
    if (PARAMS_READ_CH_FROM_FILE) {
        adjacencyListCH = IO::readGraphFromFile(PATH_CH_GRAPHS PARAMS_GRAPH_NAME CH_GRAPHS_EXTENSION);
    } else {
        CH ch(adjacencyList);
        adjacencyListCH = ch.preprocess();
        GraphUtils::printGraphProperties(adjacencyListCH);
        if (PARAMS_WRITE_CH_TO_FILE) IO::writeGraphToFile(PATH_CH_GRAPHS PARAMS_GRAPH_NAME CH_GRAPHS_EXTENSION, adjacencyListCH);
    }

    if (PARAMS_QUERY_BENCHMARK) Benchmark::queryBenchmark(adjacencyList, adjacencyListCH, PARAMS_NB_RUNS_QUERY_BENCHMARK);
    
    clock.lap(true);
    
}

