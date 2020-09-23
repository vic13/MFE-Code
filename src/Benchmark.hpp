

class Benchmark {
public:
    /// Perform the specified number of random queries, both with Dijkstra's algorithm and CH query algorithm. Verify if both give the same path cost. Return the averaged results.
    static string queryBenchmark(vector<vector<Edge>> adjacencyList, vector<vector<CHQueryEdge>> adjacencyListCH, int nbRuns = PARAMS_NB_RUNS_QUERY_BENCHMARK) {
        auto avgDijkstra = 0.0;
        auto avgDijkstraCH = 0.0;
        int avgSearchSpaceDijkstra = 0;
        int avgSearchSpaceDijkstraCH = 0;
        int avgRelaxedDijkstra = 0;
        int avgRelaxedCH = 0;
        
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
            avgRelaxedDijkstra += dijkstra.getRelaxedEdges();
            avgRelaxedCH += dijkstraCH.getRelaxedEdges();

            if (dijkstra.getPathWeight() != dijkstraCH.getPathWeight()) {
                cout << "Wrong for s : " << s << " and t : " << t << " - w1 : " << dijkstra.getPathWeight() << " and w2 : " << dijkstraCH.getPathWeight() << " dw : " << dijkstraCH.getPathWeight()-dijkstra.getPathWeight() << endl;
                exit(0);
            }
            
        }
        stringstream ss(stringstream::in | stringstream::out);
        ss << avgDijkstra/(1000.0*nbRuns) << " ";                                       // Dijkstra time
        ss << avgDijkstraCH/(1000.0*nbRuns) << " ";                                     // CH time
        ss << avgDijkstra/avgDijkstraCH << " ";                                         // Speed-up
        ss << (float)avgSearchSpaceDijkstra/(nbRuns) << " ";                            // Dijkstra Search space
        ss << (float)avgSearchSpaceDijkstraCH/(nbRuns) << " ";                          // CH Search space
        ss << (float)avgSearchSpaceDijkstra/(float)avgSearchSpaceDijkstraCH << " ";     // Search space Gain
        ss << (float)avgRelaxedDijkstra/(nbRuns) << " ";                                // Dijkstra Relaxed edges
        ss << (float)avgRelaxedCH/(nbRuns) << endl;                                     // CH Relaxed edges
        return ss.str();
    }

    /// Perform the specified number of random queries, both with Dijkstra's time-dependent algorithm and TCH query algorithm. Verify if both give the same path cost. Return the averaged results.
    static string queryBenchmarkTD(vector<vector<TDEdge>> adjacencyList, vector<vector<TCHQueryEdge>> adjacencyListCH, int nbRuns = PARAMS_NB_RUNS_QUERY_BENCHMARK) {
        auto avgDijkstra = 0.0;
        auto avgDijkstraCH = 0.0;
        auto avgBackwardSearch = 0.0;
        int avgSearchSpaceDijkstra = 0;
        int avgSearchSpaceDijkstraCH = 0;
        int avgRelaxedDijkstra = 0;
        int avgRelaxedCH = 0;
        int avgSearchSpaceBackwardSearch = 0;

        for (int i = 0; i<nbRuns; i++) {
            int s = Random::randomInt(adjacencyList.size());
            int t = Random::randomInt(adjacencyList.size());
            int startingTime = Random::randomInt(TTF::period);

            DijkstraTD dijkstra(adjacencyList, s, t, startingTime);
            DijkstraTCH dijkstraCH(adjacencyListCH, s, t, startingTime);
            
            auto t1 = std::chrono::high_resolution_clock::now();
            bool c1 = dijkstra.compute();
            auto t2 = std::chrono::high_resolution_clock::now();
            int backwardSearchSpace = dijkstraCH.markReachable();
            auto t3 = std::chrono::high_resolution_clock::now();
            bool c2 = dijkstraCH.compute();
            auto t4 = std::chrono::high_resolution_clock::now();

            if (!c1 || !c2) {cout << "Problem Benchmark : No solution found" << endl; exit(0);}

            avgDijkstra += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            avgDijkstraCH += std::chrono::duration_cast<std::chrono::microseconds>(t4 - t2).count();
            avgBackwardSearch += std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
            avgSearchSpaceDijkstra += dijkstra.getSearchSpace();
            avgSearchSpaceDijkstraCH += dijkstraCH.getSearchSpace();
            avgRelaxedDijkstra += dijkstra.getRelaxedEdges();
            avgRelaxedCH += dijkstraCH.getRelaxedEdges();
            avgSearchSpaceBackwardSearch += backwardSearchSpace;

            if (dijkstra.getPathWeight() != dijkstraCH.getPathWeight()) {
                cout << "Wrong for s : " << s << " and t : " << t << " - w1 : " << dijkstra.getPathWeight() << " and w2 : " << dijkstraCH.getPathWeight() << " dw : " << dijkstraCH.getPathWeight()-dijkstra.getPathWeight() << endl;
                exit(0);
            }
            
        }
        stringstream ss(stringstream::in | stringstream::out);
        ss << avgDijkstra/(1000.0*nbRuns) << " ";                                           // Dijkstra time
        ss << avgDijkstraCH/(1000.0*nbRuns) << " ";                                         // CH time
        ss << avgBackwardSearch/(1000.0*nbRuns) << " ";                                     // Backward search time
        ss << avgDijkstra/avgDijkstraCH << " ";                                             // Speed-up
        ss << (float)avgSearchSpaceDijkstra/(nbRuns) << " ";                                // Dijkstra Search space
        ss << (float)avgSearchSpaceDijkstraCH/(nbRuns) << " ";                              // CH Search space
        ss << (float)avgSearchSpaceBackwardSearch/(nbRuns) << " ";                          // Backward search search space
        ss << (float)avgSearchSpaceDijkstra/(float)avgSearchSpaceDijkstraCH << " ";         // Search space Gain
        ss << (float)avgRelaxedDijkstra/(nbRuns) << " ";                                    // Dijkstra Relaxed edges
        ss << (float)avgRelaxedCH/(nbRuns) << endl;                                         // CH Relaxed edges
        return ss.str();
    }

    /// Compile the preprocessing results based on the input graph and the results of CH preprocessing
    template <typename T_Edge, typename T_CHQueryEdge>
    static string preprocessingBenchmark(vector<vector<T_Edge>> adjacencyList, vector<vector<T_CHQueryEdge>> adjacencyListCH, float preprocessingTime, float maxAvgDegree) {
        int sizeBase = GraphUtils::getSize(adjacencyList);
        int sizeCH = GraphUtils::getSize(adjacencyListCH);
        stringstream ss(stringstream::in | stringstream::out);
        ss << adjacencyList.size() << " ";                                  // Nb vertices
        ss << GraphUtils::getNbEdges(adjacencyList) << " ";                 // Nb edges
        ss << GraphUtils::getNbEdges(adjacencyListCH) << " ";               // Nb edges CH
        ss << sizeBase << " ";                                              // Size (B)
        ss << sizeCH << " ";                                                // Size CH (B)
        ss << maxAvgDegree << " ";                                          // Maximum average degree (CH)
        ss << (float)(sizeCH-sizeBase)/(float)adjacencyList.size() << " ";  // Memory Overhead
        ss << preprocessingTime << " ";                                     // Preprocessing time
        return ss.str();
    }

    /// Perform the first experiment (on a single-modal car network). Import the OSM graph, build the CH, run the benchmarking, and save results on file.
    static void exp1() {
        OSMGraph osmGraph(PATH_OSM_GRAPHS PARAMS_GRAPH_NAME OSM_GRAPHS_EXTENSION);
        vector<vector<Edge>> adjacencyList = osmGraph.build(true);
        osmGraph.printImportStats();

        CH ch(adjacencyList);
        vector<vector<CHQueryEdge>> adjacencyListCH = ch.preprocess();

        stringstream ss(stringstream::in | stringstream::out);
        ss << "nb-vertices nb-edges nb-edges-CH size size_CH max-avg-d m-ovhd pr-t q-t-dijk q-t-CH q-t-up q-v-dijk q-v-CH q-v-up rel-dijk rel-CH" << endl;
        ss << Benchmark::preprocessingBenchmark(adjacencyList, adjacencyListCH, ch.getPreprocessingTime(), ch.getMaxAvgDegree());
        ss << Benchmark::queryBenchmark(adjacencyList, adjacencyListCH);
        stringstream filePath(stringstream::in | stringstream::out);
        filePath << PATH_BENCHMARKS << PARAMS_EXP_NB << "/" << PARAMS_GRAPH_NAME << BENCHMARKS_EXTENSION;
        IO::writeToFile(filePath.str(), ss.str());
    }

    /// Perform the second experiment (on multi-modal public transport networks). For each parameter configuration : construct the graph, build the CH, run the benchmarking, and save results on file.
    static void exp2() {
        OSMGraph osmGraph(PATH_OSM_GRAPHS PARAMS_GRAPH_NAME OSM_GRAPHS_EXTENSION);
        vector<vector<Edge>> adjacencyList = osmGraph.build(false);
        osmGraph.printImportStats();
        vector<pair<float, float>> coords = osmGraph.getVerticesCoordinates();

        vector<int> nbs({0, 10, 50, 100, 200});             // Number of added lines
        vector<float> speeds({0.1, 15, 30, 90, 1e10});      // Speed of the "hop" edges
        for (float speed_kmh : speeds) {
            stringstream ss(stringstream::in | stringstream::out);
            ss << "nb nb-vertices nb-edges nb-edges-CH size size_CH max-avg-d m-ovhd pr-t q-t-dijk q-t-CH q-t-up q-v-dijk q-v-CH q-v-up rel-dijk rel-CH" << endl;
            for (int nb : nbs) {
                cout << "speed : " << speed_kmh << " nb : " << nb << endl;
                vector<vector<Edge>> adjacencyListMulti = adjacencyList;
                int addedLinks = 0;
                while (addedLinks < nb) {
                    int boardVertexDown = Random::randomInt(adjacencyList.size());
                    int alightVertexDown = Random::randomInt(adjacencyList.size());
                    if (boardVertexDown == alightVertexDown) continue;
                    float distance_km = osmGraph.distanceLatLong(coords[boardVertexDown].first, coords[boardVertexDown].second, coords[alightVertexDown].first, coords[alightVertexDown].second);
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
                ss << nb << " " << Benchmark::preprocessingBenchmark(adjacencyListMulti, adjacencyListCH, ch.getPreprocessingTime(), ch.getMaxAvgDegree());
                ss << Benchmark::queryBenchmark(adjacencyListMulti, adjacencyListCH);
            }
            stringstream filePath(stringstream::in | stringstream::out);
            filePath << PATH_BENCHMARKS << PARAMS_EXP_NB << "/" << PARAMS_GRAPH_NAME << "-s" << std::to_string(speed_kmh) << BENCHMARKS_EXTENSION;
            IO::writeToFile(filePath.str(), ss.str());
        }
    }

    /// Perform the third experiment (on multi-modal time-dependent public transport networks). For each parameter configuration : construct the graph, build the CH, run the benchmarking, and save results on file.
    static void exp3() {
        OSMGraph osmGraph(PATH_OSM_GRAPHS PARAMS_GRAPH_NAME OSM_GRAPHS_EXTENSION);
        vector<vector<Edge>> adjacencyList = osmGraph.build(false);
        osmGraph.printImportStats();
        vector<pair<float, float>> coords = osmGraph.getVerticesCoordinates();
        vector<vector<TDEdge>> adjacencyListTD = GraphUtils::convertToTDGraph(adjacencyList);

        vector<int> nbs({0, 10, 50, 100, 200});     // Number of added lines
        float speed_kmh = 30;
        vector<int> ttfTypes({0,1});                // Weight type for the board edge : 0 -> constant (null), 1 -> waiting times TTF
        for (int ttfType : ttfTypes) {
            stringstream ss(stringstream::in | stringstream::out);
            ss << "nb nb-vertices nb-edges nb-edges-CH size size_CH max-avg-d m-ovhd pr-t q-t-dijk q-t-CH q-t-bw q-t-up q-v-dijk q-v-CH q-v-bw q-v-up rel-dijk rel-CH" << endl;
            for (int nb : nbs) {
                cout << "type : " << ttfType << " nb : " << nb << endl;
                vector<vector<TDEdge>> adjacencyListMulti = adjacencyListTD;
                int addedLinks = 0;
                while (addedLinks < nb) {
                    int boardVertexDown = Random::randomInt(adjacencyListTD.size());
                    int alightVertexDown = Random::randomInt(adjacencyListTD.size());
                    if (boardVertexDown == alightVertexDown) continue;
                    float distance_km = osmGraph.distanceLatLong(coords[boardVertexDown].first, coords[boardVertexDown].second, coords[alightVertexDown].first, coords[alightVertexDown].second);
                    int time = (int)(3600.0f*distance_km/speed_kmh);
                    adjacencyListMulti.push_back({});
                    adjacencyListMulti.push_back({});
                    int boardVertexUp = adjacencyListMulti.size()-2;
                    int alightVertexUp = adjacencyListMulti.size()-1;
                    TDEdge boardEdge(boardVertexUp, TTF(0));
                    if (ttfType==1) boardEdge = TDEdge(boardVertexUp, TTF::randomTransitTTF(15*60)); // Max waiting time : 15 min
                    TDEdge hopEdge(alightVertexUp, TTF(time));
                    TDEdge alightEdge(alightVertexDown, TTF(0));
                    adjacencyListMulti[boardVertexDown].push_back(boardEdge);
                    adjacencyListMulti[boardVertexUp].push_back(hopEdge);
                    adjacencyListMulti[alightVertexUp].push_back(alightEdge);
                    addedLinks++;
                }
                
                TCH tch(adjacencyListMulti);
                vector<vector<TCHQueryEdge>> adjacencyListTCH = tch.preprocess();
                ss << nb << " " << Benchmark::preprocessingBenchmark(adjacencyListMulti, adjacencyListTCH, tch.getPreprocessingTime(), tch.getMaxAvgDegree());
                ss << Benchmark::queryBenchmarkTD(adjacencyListMulti, adjacencyListTCH);
            }
            stringstream filePath(stringstream::in | stringstream::out);
            filePath << PATH_BENCHMARKS << PARAMS_EXP_NB << "/" << PARAMS_GRAPH_NAME << "-type" << std::to_string(ttfType) << BENCHMARKS_EXTENSION;
            IO::writeToFile(filePath.str(), ss.str());
        }
    }

    /// Perform the fourth experiment (on multi-modal station-based network). Construct the graph, build the CH, run the benchmarking, and save results on file.
    static void exp4() {
        OSMGraph osmGraph(PATH_OSM_GRAPHS PARAMS_GRAPH_NAME OSM_GRAPHS_EXTENSION);
        vector<vector<Edge>> adjacencyList = osmGraph.build(false);
        osmGraph.printImportStats();
        vector<pair<float, float>> coords = osmGraph.getVerticesCoordinates();

        // Get stations coordinates and compute closest vertices
        vector<pair<float, float>> villoCoords = osmGraph.getVilloStationsCoordinates();
        vector<bool> villoVertices = vector<bool>(adjacencyList.size(), false);
        for (auto& villoCoord : villoCoords) {
            int closest = -1;
            float closestDistance = -1;
            for (int v = 0; v<coords.size(); v++) {
                float distance = osmGraph.distanceLatLong(coords[v].first, coords[v].second, villoCoord.first, villoCoord.second);
                if (distance < closestDistance || closest == -1) {
                    closestDistance = distance;
                    closest = v;
                }
            }
            villoVertices[closest] = true;
        }

        // Duplicate walk graph, change weights to match bike speeds
        int n = adjacencyList.size();
        for (int v = 0; v<n; v++) {
            auto& edges = adjacencyList[v];
            vector<Edge> bikeEdges;
            for (auto& edge : edges) {
                bikeEdges.push_back(Edge(edge.getDestinationVertex() + n, edge.getWeight()/3));
            }
            adjacencyList.push_back(bikeEdges);
        }
        
        // Add board and alight edges
        for (int v = 0; v<n; v++) {
            if (villoVertices[v]) {
                adjacencyList[v].push_back(Edge(v+n, 60)); // board
                adjacencyList[v+n].push_back(Edge(v, 60)); // alight
            }
        }

        // Benchmark
        CH ch(adjacencyList);
        vector<vector<CHQueryEdge>> adjacencyListCH = ch.preprocess();
        stringstream ss(stringstream::in | stringstream::out);
        ss << "nb-vertices nb-edges nb-edges-CH size size_CH max-avg-d m-ovhd pr-t q-t-dijk q-t-CH q-t-up q-v-dijk q-v-CH q-v-up rel-dijk rel-CH" << endl;
        ss << Benchmark::preprocessingBenchmark(adjacencyList, adjacencyListCH, ch.getPreprocessingTime(), ch.getMaxAvgDegree());
        ss << Benchmark::queryBenchmark(adjacencyList, adjacencyListCH);
        stringstream filePath(stringstream::in | stringstream::out);
        filePath << PATH_BENCHMARKS << PARAMS_EXP_NB << "/" << PARAMS_GRAPH_NAME << BENCHMARKS_EXTENSION;
        IO::writeToFile(filePath.str(), ss.str());
    }

};