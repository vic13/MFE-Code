

class Benchmark {
public:
    static void queryBenchmark(vector<vector<Edge>> adjacencyList, vector<vector<CHQueryEdge>> adjacencyListCH, int nbRuns, string additionalInfo = "") {
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
        stringstream filePath(stringstream::in | stringstream::out);
        if (PARAMS_QUERY_STALL) {
            filePath << PATH_BENCHMARKS PARAMS_GRAPH_NAME "-query" << additionalInfo << BENCHMARKS_EXTENSION;
        } else {
            filePath << PATH_BENCHMARKS PARAMS_GRAPH_NAME "-query-no_stall" << additionalInfo << BENCHMARKS_EXTENSION;
        }
        IO::writeToFile(filePath.str(), ss.str());
    }

    static void preprocessingBenchmark(vector<vector<Edge>> adjacencyList, vector<vector<CHQueryEdge>> adjacencyListCH, float preprocessingTime, string additionalInfo = "") {
        int sizeBase = GraphUtils::getSize(adjacencyList);
        int sizeCH = GraphUtils::getSize(adjacencyListCH);
        stringstream ss(stringstream::in | stringstream::out);
        ss << "Nb vertices : " << adjacencyList.size() << endl;
        ss << "Nb edges base graph : " << GraphUtils::getNbEdges(adjacencyList) << endl;
        ss << "Nb edges CH : " << GraphUtils::getNbEdges(adjacencyListCH) << endl;
        ss << "Size base graph (B) : " << sizeBase << endl;
        ss << "Size CH graph (B) : " << sizeCH << endl;
        ss << "Memory Overhead (B/vertex) : " << (float)(sizeCH-sizeBase)/(float)adjacencyList.size() << endl;
        ss << "Preprossessing time (s) : " << preprocessingTime << endl;
        stringstream filePath(stringstream::in | stringstream::out);
        filePath << PATH_BENCHMARKS PARAMS_GRAPH_NAME "-preprocessing" << additionalInfo << BENCHMARKS_EXTENSION;
        IO::writeToFile(filePath.str(), ss.str());
    }

    static void exp1() {
        OSMGraph osmGraph(PATH_OSM_GRAPHS PARAMS_GRAPH_NAME OSM_GRAPHS_EXTENSION);
        vector<vector<Edge>> adjacencyList = osmGraph.build(true);
        osmGraph.printImportStats();
        vector<vector<CHQueryEdge>> adjacencyListCH;
        if (PARAMS_READ_CH_FROM_FILE) {
            adjacencyListCH = IO::readGraphFromFile(PATH_CH_GRAPHS PARAMS_GRAPH_NAME CH_GRAPHS_EXTENSION);
        } else {
            CH ch(adjacencyList);
            adjacencyListCH = ch.preprocess();
            if (PARAMS_WRITE_CH_TO_FILE) IO::writeGraphToFile(PATH_CH_GRAPHS PARAMS_GRAPH_NAME CH_GRAPHS_EXTENSION, adjacencyListCH);
            if (PARAMS_PREPROCESSING_BENCHMARK) Benchmark::preprocessingBenchmark(adjacencyList, adjacencyListCH, ch.getPreprocessingTime());
        }

        if (PARAMS_QUERY_BENCHMARK) Benchmark::queryBenchmark(adjacencyList, adjacencyListCH, PARAMS_NB_RUNS_QUERY_BENCHMARK);
    }

    static void exp2() {
        OSMGraph osmGraph(PATH_OSM_GRAPHS PARAMS_GRAPH_NAME OSM_GRAPHS_EXTENSION);
        vector<vector<Edge>> adjacencyList = osmGraph.build(false);
        osmGraph.printImportStats();
        vector<pair<float, float>> coord = osmGraph.getVerticesCoordinates();

        vector<int> nbs({0, 10, 50, 100, 200});
        vector<float> speeds({0.1, 5, 15, 30, 90, 1e10});
        for (int nb : nbs) {
            for (float speed_kmh : speeds) {
                vector<vector<Edge>> adjacencyListMulti = adjacencyList;
                int addedLinks = 0;
                while (addedLinks < nb) {
                    int boardVertexDown = Random::randomInt(adjacencyList.size());
                    int alightVertexDown = Random::randomInt(adjacencyList.size());
                    if (boardVertexDown == alightVertexDown) continue;
                    float distance_km = osmGraph.distanceLatLong(coord[boardVertexDown].first, coord[boardVertexDown].second, coord[alightVertexDown].first, coord[alightVertexDown].second);
                    int time = (int)(3600.0f*distance_km/speed_kmh);
                    adjacencyListMulti.push_back({});
                    adjacencyListMulti.push_back({});
                    int boardVertexUp = adjacencyListMulti.size()-2;
                    int alightVertexUp = adjacencyListMulti.size()-1;
                    Edge boardEdge(boardVertexUp, 0);
                    Edge hopEdge(alightVertexUp, time);
                    Edge alightEdge(alightVertexDown, 0);
                    adjacencyListMulti[boardVertexDown].push_back(boardEdge);
                    adjacencyListMulti[boardVertexUp].push_back(hopEdge);
                    adjacencyListMulti[alightVertexUp].push_back(alightEdge);
                    addedLinks++;
                }
                
                CH ch(adjacencyListMulti);
                vector<vector<CHQueryEdge>> adjacencyListCH = ch.preprocess();
                string additionalInfo = "-nb"+std::to_string(nb)+"-s"+std::to_string(speed_kmh);
                Benchmark::preprocessingBenchmark(adjacencyListMulti, adjacencyListCH, ch.getPreprocessingTime(), additionalInfo);
                Benchmark::queryBenchmark(adjacencyListMulti, adjacencyListCH, PARAMS_NB_RUNS_QUERY_BENCHMARK, additionalInfo);
            }
        }
    }

    static void exp3() {
        // vector<vector<TDEdge>> adjacencyListTD = GraphUtils::convertToTDGraph(adjacencyList);
        // TCH tch(adjacencyListTD);
        // vector<vector<TCHQueryEdge>> adjacencyListTCH = tch.preprocess();
        // GraphUtils::printGraphProperties(adjacencyListTCH);
        // return 0;

        // for (int i = 0; i<10; i++) {
        //     int s = Random::randomInt(adjacencyList.size());
        //     int t = Random::randomInt(adjacencyList.size());
        //     float startingTime = TTF::period*Random::random01();
        //     DijkstraTD dijkstraTD(adjacencyListTD, s, t, startingTime);
        //     dijkstraTD.compute();
        //     cout << dijkstraTD.getPathWeight() << endl;

        //     DijkstraTCH dijkstraTCH(adjacencyListTCH, s, t, startingTime);
        //     dijkstraTCH.markReachable();
        //     dijkstraTCH.compute();
        //     cout << dijkstraTCH.getPathWeight() << endl;
        // }
        // return 0;

        // int r = 10;
        // TTF f1({
        //     make_pair(0,5*10*r),make_pair(100,5*9*r),make_pair(101,5*9.5*r),make_pair(200,5*8*r),make_pair(TTF::period,5*9*r)
        // });
        // TTF f2({
        //     make_pair(0,15*r),make_pair(30,25*r),make_pair(TTF::period,10*r)
        // });
        // TTF f1 = TTF::randomTransitTTF();
        // TTF f2 = TTF::randomTransitTTF();
        // return 0;
        
        // TTF f0 = TTF::randomTransitTTF();
        // for (int i=0; i<=986; i++) {
        //     cout << i << " : " << f0.getPoints().size() << endl;
        //     TTF f1 = TTF::randomTransitTTF();
        //     if (Random::random01() < 0.5) {
        //         f1 = TTF(1+Random::randomInt(100));
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
    }

};