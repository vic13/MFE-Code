

class Benchmark {
public:
    static void queryBenchmark(vector<vector<Edge>> adjacencyList, vector<vector<CHQueryEdge>> adjacencyListCH, int nbRuns) {
        bool correct = true;

        GraphUtils::printGraphProperties(adjacencyList);
        GraphUtils::printGraphProperties(adjacencyListCH);

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
                correct = false;
            }
            
        }
        cout << "------- Dijkstra vs CH Query -------" << endl;
        correct ? cout << "Correct" << endl : cout << "!!! NOT CORRECT !!!" << endl;
        cout << "Average time dijkstra : " << avgDijkstra/(1000.0*nbRuns) << " ms" << endl;
        cout << "Average time dijkstraCH : " << avgDijkstraCH/(1000.0*nbRuns) << " ms" << endl;
        cout << "Speed-up : " << avgDijkstra/avgDijkstraCH << endl;
        cout << "Average search space dijkstra : " << (float)avgSearchSpaceDijkstra/(nbRuns) << " settled nodes" << endl;
        cout << "Average search space dijkstraCH : " << (float)avgSearchSpaceDijkstraCH/(nbRuns) << " settled nodes" << endl;
        cout << "Search space factor : " << (float)avgSearchSpaceDijkstra/(float)avgSearchSpaceDijkstraCH << endl;
        cout << "------------------------------------" << endl;
    }

};