template <class T_Edge, class T_CHEdge, class T_CHQueryEdge>
class CHTemplate {
/* This class implements Contracion Hierarchies preprocessing functions */
public:
    CHTemplate(vector<vector<T_Edge>> inputGraph) : graph(inputGraph) {
        this->n = inputGraph.size();
        this->deletedNeighbours = vector<int>(this->n, 0);
        this->vertexWeights = vector<float>(this->n, -1);
        this->vertexWeightsR = vector<float>(this->n, -1);
        this->hops = vector<int>(this->n, 0);
        this->hopsR = vector<int>(this->n, 0);
    }

    vector<vector<T_CHQueryEdge>> preprocess() {
        auto t1 = std::chrono::high_resolution_clock::now();
        this->buildVertexOrdering();
        this->constructCH();
        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        std::cout << "CH duration : " << duration/1000000.0 << " seconds" << endl;
        vector<vector<T_CHQueryEdge>> g_star = convertToSearchGraph(this->g_H);
        return g_star;
    }

protected:
    CHGraph<T_Edge, T_CHEdge> graph;
    int n;
    set<pair<float, int>> vertexOrdering;
    vector<int> vertexOrderMap;
    vector<float> vertexOrderingScores; // Used to find elements in vertexOrdering
    vector<int> deletedNeighbours;
    CHGraph<T_Edge, T_CHEdge> g_H = graph; // CH graph
    CHGraph<T_Edge, T_CHEdge> g_R = graph; // Remaining graph
    vector<float> vertexWeights; // Dijkstra weights (for memory optimisation)
    vector<float> vertexWeightsR; // Reference Dijkstra weights (for memory optimisation)
    vector<int> hops;
    vector<int> hopsR;
    vector<int> hopLimits = vector<int>({1, 2, 3, 5});
    vector<float> hopTresholds = vector<float>({3.3, 7, 8});
    int hopIndex = 0;

    void buildVertexOrdering() {
        for (int vertexNb = 0; vertexNb < this->n; vertexNb++) {
            float priorityScore = calcPriorityScore(vertexNb);
            vertexOrderingScores.push_back(priorityScore);
            vertexOrdering.insert(make_pair(priorityScore, vertexNb));
        }
        this->vertexOrderMap = vector<int>(this->n, -1);
    }

    bool updateOrdering(int vertexNb) {   // Update ordering for the specified vertex, and returns true if it is on top of the queue
        pair<float, int> current = make_pair(vertexOrderingScores[vertexNb], vertexNb);
        vertexOrdering.erase(vertexOrdering.find(current));
        float priorityScore = calcPriorityScore(vertexNb);
        vertexOrderingScores[vertexNb] = priorityScore;
        return (vertexOrdering.insert(make_pair(priorityScore, vertexNb)).first == vertexOrdering.begin());
    }

    virtual float calcPriorityScore(int vertexNb) {
        vector<int> simulResults = contractVertex(vertexNb, true);
        int addedShortcuts = simulResults[0];
        int s = simulResults[1]; // Search space
        int incidentEdges = g_R.getIngoingEdges(vertexNb).size() + g_R.getOutgoingEdges(vertexNb).size();
        int e = addedShortcuts - incidentEdges; // Edge difference
        int d = deletedNeighbours[vertexNb];// Deleted neighbours
        return 190*e + 120*d + 10*s;
    }

    void constructCH() {
        int order = 0;
        while (!vertexOrdering.empty()) {
            int priorityVertex = (*(vertexOrdering.begin())).second;
            // Lazy update : update ordering, and sample another vertex if it is no longer on top of the queue
            if (!updateOrdering(priorityVertex)) {
                //cout << "continue" << endl;
                continue;
            }
            vertexOrdering.erase(vertexOrdering.begin());
            contractVertex(priorityVertex, false);
            updateDeletedNeighbours(priorityVertex); // Priority term
            g_R.removeVertex(priorityVertex);
            vertexOrderMap[priorityVertex] = order;
            
            // Update hop limit
            if ((hopIndex<hopTresholds.size()) && (g_R.getAverageDegree() >= hopTresholds[hopIndex])) {
                hopIndex++;
                cout << "Swiched to hop limit : " << hopLimits[hopIndex] << " (order : " << order << ")" << endl;
            }
            // Neighbours update
            unordered_set<int> neighbours = g_R.getNeighbours(priorityVertex);
            for (auto& neighbour : neighbours) {   
                updateOrdering(neighbour);
            }

            // Print progress
            if (100*order/(this->n-1) > 100*(order-1)/(this->n-1)) {cout<<"Contraction progress : "<<100*order/(this->n-1)<<" %\r"; cout.flush();}
            order++;
        }
    }

    void updateDeletedNeighbours(int deletedVertex) {
        for (auto& neighbour : g_R.getNeighbours(deletedVertex)) {
            this->deletedNeighbours[neighbour]++;
        }
    }

    vector<vector<T_CHQueryEdge>> convertToSearchGraph(CHGraph<T_Edge, T_CHEdge> g_H) {
        vector<vector<T_CHQueryEdge>> g_star = vector<vector<T_CHQueryEdge>>(this->n, vector<T_CHQueryEdge>());
        for (int u = 0; u<n; u++) {
            vector<T_CHEdge*> edgePtrs = g_H.getIncidenceList()[u].first;
            for (auto& edgePtr : edgePtrs) {
                int v = edgePtr->getDestinationVertex();
                bool direction = (vertexOrderMap[u] < vertexOrderMap[v]);
                if (direction) {
                    g_star[u].push_back(T_CHQueryEdge(v, edgePtr->getWeight(), direction));
                } else {
                    g_star[v].push_back(T_CHQueryEdge(u, edgePtr->getWeight(), direction));
                }
                
            }
        }
        return g_star;
    }

    virtual vector<int> contractVertex(int vertexNb, bool simulation) = 0;
};
