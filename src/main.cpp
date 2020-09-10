#include <vector>
#include <iostream>
#include <chrono>
#include <set>
#include <queue>


using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::make_pair;
using std::string;
using std::set;
using std::queue;

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
#include "CH/CHQuery.hpp"
#include "CH/TCHQuery.hpp"
#include "Benchmark.hpp"

int main() {
    cout << endl << "Hello world !" << endl << endl;
    Random::initRandom();
    cout << std::setprecision(9) << endl;
    Clock clock("Program duration", false);

    // int r = 10;
    // TTF f1({
    //     make_pair(0,5*10*r),make_pair(100,5*9*r),make_pair(101,5*9.5*r),make_pair(200,5*8*r),make_pair(TTF::period,5*9*r)
    // });
    // TTF f2({
    //     make_pair(0,15*r),make_pair(30,25*r),make_pair(TTF::period,10*r)
    // });
    // TTF f1 = TTF::randomTransitTTF();
    // TTF f2 = TTF::randomTransitTTF();
    // View::displayTTF({f1,f2});
    // TTF f3 = TTF::minimum(f1,f2);
    // cout << "valid : " << f1.isTransitTTF() << endl;
    // cout << "valid : " << f2.isTransitTTF() << endl;
    // TTF f3 = TTF::chaining(f1,f2);
    // View::displayTTF({f1, f2, f3});
    // cout << "valid : " << f3.isTransitTTF() << endl;
    // return 0;
    
    // TTF f0 = TTF::randomTransitTTF();
    // cout << f0.isTransitTTF() << endl;
    // for (int i=0; i<=986; i++) {
    //     cout << i << " : " << f0.getPoints().size() << endl;
    //     TTF f1 = TTF::randomTransitTTF();
    //     if (Random::random01() < 0.5) {
    //         f1 = TTF(1+Random::randomInt(100));
    //         cout << f1.getPoints()[0].second << endl;
    //         cout << f0.getPoints()[0].second << endl;
    //     }
    //     float r = Random::random01();
    //     if (r < 0.3) {
    //         f0 = TTF::chaining(f0, f1);
    //     } else if (r<0.6) {
    //         f0 = TTF::chaining(f1, f0);
    //     } else {
    //         f0 = TTF::minimum(f0, f1);
    //     }
    // }
    // TTF f0 = TTF::randomTransitTTF();
    // cout << f0.evaluate(10) << endl;
    // View::displayTTF({f0});
    // return 0;

    OSMGraph osmGraph(PATH_OSM_GRAPHS PARAMS_GRAPH_NAME OSM_GRAPHS_EXTENSION);
    vector<vector<Edge>> adjacencyList = osmGraph.build();
    osmGraph.printImportStats();
    GraphUtils::printGraphProperties(adjacencyList);
    // #if (PARAMS_VIEW) 
    //     View::displayNetwork(adjacencyList, osmGraph.getVerticesCoordinates());
    // #endif

    vector<vector<TDEdge>> adjacencyListTD = GraphUtils::convertToTDGraph(adjacencyList);
    cout << "a" << endl;
    TCH tch(adjacencyListTD);
    cout << "b" << endl;
    vector<vector<TCHQueryEdge>> adjacencyListTCH = tch.preprocess();
    cout << "c" << endl;
    GraphUtils::printGraphProperties(adjacencyListTCH);
    return 0;

    // for (int i = 0; i<10; i++) {
    //     int s = Random::randomInt(adjacencyList.size());
    //     int t = Random::randomInt(adjacencyList.size());
    //     float startingTime = TTF::period*Random::random01();
    //     DijkstraTD dijkstraTD(adjacencyListTD, s, t, startingTime);
    //     cout << dijkstraTD.compute() << endl;
    //     cout << dijkstraTD.getPathWeight() << endl;

    //     DijkstraTCH dijkstraTCH(adjacencyListTCH, s, t, startingTime);
    //     dijkstraTCH.markReachable();
    //     cout << dijkstraTCH.compute() << endl;
    //     cout << dijkstraTCH.getPathWeight() << endl;
    // }
    // return 0;

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

