

class Benchmark {
public:
    static void queryBenchmark(vector<vector<Edge>> adjacencyList, vector<vector<CHQueryEdge>> adjacencyListCH, int nbRuns) {
        bool correct = true;

        auto avgDijkstra = 0.0;
        auto avgDijkstraCH = 0.0;
        int avgSearchSpaceDijkstra = 0;
        int avgSearchSpaceDijkstraCH = 0;

        for (int i = 0; i<nbRuns; i++) {
            int s = Random::randomInt(adjacencyList.size());
            int t = Random::randomInt(adjacencyList.size());

            Dijkstra dijkstra(adjacencyList, s, t);
            CHQuery dijkstraCH(adjacencyListCH, s, t);
            
            auto t1 = std::chrono::high_resolution_clock::now();
            bool c1 = dijkstra.compute();
            auto t2 = std::chrono::high_resolution_clock::now();
            bool c2 = dijkstraCH.compute();
            auto t3 = std::chrono::high_resolution_clock::now();

            if (!c1 || !c2) {cout << "Problem Benchmark : No solution found" << endl; exit(0);}

            avgDijkstra += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            avgDijkstraCH += std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
            avgSearchSpaceDijkstra += dijkstra.getSearchSpace();
            avgSearchSpaceDijkstraCH += dijkstraCH.getSearchSpace();

            if (dijkstra.getPathWeight() != dijkstraCH.getPathWeight()) {
                cout << "Wrong for s : " << s << " and t : " << t << " - w1 : " << dijkstra.getPathWeight() << " and w2 : " << dijkstraCH.getPathWeight() << " dw : " << dijkstraCH.getPathWeight()-dijkstra.getPathWeight() << endl;
                exit(0);
            }
            
        }
        stringstream ss(stringstream::in | stringstream::out);
        ss << "Nb runs : " << nbRuns << endl;
        ss << "Average time dijkstra (ms) : " << avgDijkstra/(1000.0*nbRuns) << endl;
        ss << "Average time CH (ms) : " << avgDijkstraCH/(1000.0*nbRuns) << endl;
        ss << "Speed-up : " << avgDijkstra/avgDijkstraCH << endl;
        ss << "Average search space dijkstra : " << (float)avgSearchSpaceDijkstra/(nbRuns) << endl;
        ss << "Average search space CH : " << (float)avgSearchSpaceDijkstraCH/(nbRuns) << endl;
        ss << "Search space factor : " << (float)avgSearchSpaceDijkstra/(float)avgSearchSpaceDijkstraCH << endl;
        IO::writeToFile(PATH_BENCHMARKS PARAMS_GRAPH_NAME "-query" BENCHMARKS_EXTENSION, ss.str());
    }

    static void preprocessingBenchmark(vector<vector<Edge>> adjacencyList, vector<vector<CHQueryEdge>> adjacencyListCH, float preprocessingTime) {
        int sizeBase = GraphUtils::getSize(adjacencyList);
        int sizeCH = GraphUtils::getSize(adjacencyListCH);
        stringstream ss(stringstream::in | stringstream::out);
        ss << "Nb vertices : " << adjacencyList.size() << endl;
        ss << "Nb edges base graph : " << GraphUtils::getNbEdges(adjacencyList) << endl;
        ss << "Nb edges CH : " << GraphUtils::getNbEdges(adjacencyListCH) << endl;
        ss << "Size base graph (B) : " << sizeBase << endl;
        ss << "Size CH graph (B) : " << sizeCH << endl;
        ss << "Memory Overhead (B/vertex) : " << (float)(sizeCH-sizeBase)/(float)adjacencyList.size() << endl;
        ss << "Preprossessing time : " << preprocessingTime << endl;
        IO::writeToFile(PATH_BENCHMARKS PARAMS_GRAPH_NAME "-preprocessing" BENCHMARKS_EXTENSION, ss.str());
    }

};