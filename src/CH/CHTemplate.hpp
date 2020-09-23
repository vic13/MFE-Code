template <class T_Edge, class T_CHEdge, class T_CHQueryEdge, class T_weight>
class CHTemplate {

public:
    CHTemplate(vector<vector<T_Edge>> inputGraph) : graph(inputGraph) {
        this->n = inputGraph.size();
        this->deletedNeighbours = vector<int>(this->n, 0);
        this->vertexWeights = vector<int>(this->n, -1);
        this->vertexWeightsR = vector<int>(this->n, -1);
        this->hops = vector<int>(this->n, 0);
        this->hopsR = vector<int>(this->n, 0);
    }

    /// Build the vertex initial vertex ordering, construct the Contraction Hierarchy, and return the search graph
    vector<vector<T_CHQueryEdge>> preprocess() {
        auto t1 = std::chrono::high_resolution_clock::now();
        this->buildVertexOrdering();
        this->constructCH();
        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        this->preprocessingTime = duration/1000000.0;
        std::cout << "CH duration : " << preprocessingTime << " seconds" << endl;
        vector<vector<T_CHQueryEdge>> g_star = convertToSearchGraph(this->g_H);
        return g_star;
    }

    float getPreprocessingTime() {
        return this->preprocessingTime;
    }

    float getMaxAvgDegree() {
        return this->maxAvgDegree;
    }

protected:
    CHGraph<T_Edge, T_CHEdge, T_weight> graph;
    int n;
    float preprocessingTime = 0;
    float maxAvgDegree = 0;
    set<pair<float, int>> vertexOrdering;
    vector<int> vertexOrderMap;
    vector<float> vertexOrderingScores; // Used to find elements in vertexOrdering
    vector<int> deletedNeighbours;
    CHGraph<T_Edge, T_CHEdge, T_weight> g_H = graph; // CH graph
    CHGraph<T_Edge, T_CHEdge, T_weight> g_R = graph; // Remaining graph
    vector<int> vertexWeights; // Dijkstra weights (for memory optimisation)
    vector<int> vertexWeightsR; // Reference Dijkstra weights (for memory optimisation)
    vector<int> hops;
    vector<int> hopsR;
    vector<int> hopLimits = vector<int>({1, 2, 3, 5});
    vector<float> hopTresholds = vector<float>({3.3, 7, 10});
    int hopIndex = 0;

    /// Build complete vertex ordering, by computing the priority score of all vertices
    void buildVertexOrdering() {
        for (int vertexNb = 0; vertexNb < this->n; vertexNb++) {
            float priorityScore = calcPriorityScore(vertexNb);
            vertexOrderingScores.push_back(priorityScore);
            vertexOrdering.insert(make_pair(priorityScore, vertexNb));
        }
        this->vertexOrderMap = vector<int>(this->n, -1);
    }

    /// Re-compute the priority score of the specified vertex and update the vertex ordering accordingly. Return 'true' if the vertex is now the first in the ordering
    bool updateOrdering(int vertexNb) {   // Update ordering for the specified vertex, and returns true if it is on top of the queue
        pair<float, int> current = make_pair(vertexOrderingScores[vertexNb], vertexNb);
        vertexOrdering.erase(vertexOrdering.find(current));
        float priorityScore = calcPriorityScore(vertexNb);
        vertexOrderingScores[vertexNb] = priorityScore;
        return (vertexOrdering.insert(make_pair(priorityScore, vertexNb)).first == vertexOrdering.begin());
    }

    /// Calculate the priority score of the specified vertex by simulating its contraction
    float calcPriorityScore(int vertexNb) {
        vector<int> simulResults = contractVertex(vertexNb, true);
        int addedShortcuts = simulResults[0];
        int s = simulResults[1]; // Search space
        int incidentEdges = g_R.getIngoingEdges(vertexNb).size() + g_R.getOutgoingEdges(vertexNb).size();
        int e = addedShortcuts - incidentEdges; // Edge difference
        int d = deletedNeighbours[vertexNb];// Deleted neighbours
        return 190*e + 120*d + 10*s;
    }

    /// Iteratively contract the vertex with lowest priority. Perform Lazy Update and Neighbours Update.
    void constructCH() {
        int order = 0;
        while (!vertexOrdering.empty()) {
            int priorityVertex = (*(vertexOrdering.begin())).second;
            // Lazy update : update ordering, and sample another vertex if it is no longer on top of the queue
            if (!updateOrdering(priorityVertex)) {
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

            // Update max avg degree
            if (g_R.getAverageDegree() > this->maxAvgDegree) this->maxAvgDegree = g_R.getAverageDegree();

            // Print progress
            if (1000*order/(this->n-1) > 1000*(order-1)/(this->n-1)) {cout<<"Contraction progress : "<<1000*order/(this->n-1)<<" ‰\r"; cout.flush();}
            order++;
        }
    }

    /// Increment the priority term 'deleted neighbours' for all neighbours of the specified vertex
    void updateDeletedNeighbours(int deletedVertex) {
        for (auto& neighbour : g_R.getNeighbours(deletedVertex)) {
            this->deletedNeighbours[neighbour]++;
        }
    }

    /// Build the search graph from the Contraction Hierarchy
    virtual vector<vector<T_CHQueryEdge>> convertToSearchGraph(CHGraph<T_Edge, T_CHEdge, T_weight> g_H) = 0;

    /// Contract or simulate the contraction of the specified vertex. If 'simulation' is 'true' (used for vertex ordering), do not actually add shortcuts. Return the number of added shortcuts and the total search space (indicating the cost of the contraction).
    virtual vector<int> contractVertex(int vertexNb, bool simulation) = 0;
};
