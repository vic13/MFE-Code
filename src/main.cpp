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
#include "TTF.hpp"
#include "Edge.hpp"
#include "Utils/GraphUtils.hpp"
#include "Utils/IO.hpp"
#include "Dijkstra.hpp"
#include "Graph_Generation/OSMGraph.hpp"
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

    if (PARAMS_EXP_NB==1) {
        Benchmark::exp1();
    } else if (PARAMS_EXP_NB==2) {
        Benchmark::exp2();
    } else if (PARAMS_EXP_NB==3) {
        Benchmark::exp3();
    } else if (PARAMS_EXP_NB==4) {
        Benchmark::exp4();
    } else {
        cout << "Invalid experiment number" << endl;
    }
    
}
