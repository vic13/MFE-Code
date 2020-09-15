

class CHQuery {
public:
    CHQuery(vector<vector<CHQueryEdge>>& graph, int s, int t) {
        this->graph = graph;
        this->t = t;
        this->s = s;
        this->vertexSet_up = initVertexSet(s);
        this->vertexSet_down = initVertexSet(t);
        this->vertexSet = &vertexSet_up;
        this->vertexWeights_up = initVertexWeights(s);
        this->vertexWeights_down = initVertexWeights(t);
        this->vertexWeights = &vertexWeights_up;
        this->vertexParents_up = initVertexParents();
        this->vertexParents_down = initVertexParents();
        this->vertexParents = &vertexParents_up;
        this->stallingDistances_up = vector<int>(graph.size(), -1);
        this->stallingDistances_down = vector<int>(graph.size(), -1);
        this->stallingDistances = &stallingDistances_up;
    }

    bool compute() {
        while (!vertexSet_up.empty() || !vertexSet_down.empty()) {
            searchSpace++;
            // Set current direction and associated vertex set, weights, and parents 
            setDirection();

            // pop first vertex in the set
            pair<int, int> visitedVertex = *(vertexSet->begin());
            vertexSet->erase(vertexSet->begin());
            int visitedVertexWeight = visitedVertex.first;
            int visitedVertexNb = visitedVertex.second;
            updateD(visitedVertexNb);

            // Check for stopping criteria
            if (checkStoppingCriteria(visitedVertexWeight)) {
                return true;
            }

            // Stall-on-demand
            #if (PARAMS_QUERY_STALL) 
                stallOnDemand(visitedVertexNb, visitedVertexWeight);
                if ((*stallingDistances)[visitedVertexNb] != -1) continue;
            #endif

            for (auto& e : graph[visitedVertexNb]) {
                if (e.isSameDirection(direction)) {
                    relaxedEdges++;
                    int neighbourCurrentWeight = (*vertexWeights)[e.getDestinationVertex()];
                    int neighbourNewWeight = visitedVertexWeight + e.getWeight();
                    if ((neighbourCurrentWeight == -1) || (neighbourNewWeight < neighbourCurrentWeight)) {    // if smaller weight was found
                        (*stallingDistances)[e.getDestinationVertex()] = -1; // Unstall
                        if (neighbourCurrentWeight != -1) {                                                              
                            // if current weight not infinite : vertex already in queue : DELETE before inserting the updated vertex
                            pair<int, int> currentNeighbour = make_pair(neighbourCurrentWeight, e.getDestinationVertex());
                            vertexSet->erase(vertexSet->find(currentNeighbour));
                        }
                        // INSERT in queue
                        pair<int, int> newNeighbour = make_pair(neighbourNewWeight, e.getDestinationVertex());
                        vertexSet->insert(newNeighbour);
                        // UPDATE weight + parent
                        (*vertexWeights)[e.getDestinationVertex()] = neighbourNewWeight;
                        (*vertexParents)[e.getDestinationVertex()] = visitedVertexNb;
                    }
                }
            }
        }
        // Stopping criterion not met
        return (d != -1);
    }

    void stallOnDemand(int u, int du) {
        if ((*stallingDistances)[u] == -1) {
            for (auto& e : graph[u]) {
                if (!e.isSameDirection(direction)) {
                    int v = e.getDestinationVertex();
                    int dv = (*vertexWeights)[v];
                    if ((dv != -1) && (dv + e.getWeight() < du)) {
                        (*stallingDistances)[u] = dv + e.getWeight();
                        // stallPropagate(u);
                        break;
                    }
                }
            }
        }
    }

    void stallPropagate(int u) {
        queue<int> q({u});
        while (!q.empty()) {
            int x = q.front();
            q.pop();
            for (auto& edge : graph[x]) {
                if (edge.isSameDirection(direction)) {
                    int v = edge.getDestinationVertex();
                    int newDistance = (*stallingDistances)[x] + edge.getWeight();
                    if (((*vertexWeights)[v] == -1) || ((*stallingDistances)[v] != -1) || ((*vertexWeights)[v] <= newDistance)) { // if unreached, already stalled, or no better distance
                        continue;
                    }
                    (*stallingDistances)[v] = newDistance;
                    q.push(v);
                }
            }
        }
    }

    int getPathWeight() {
        return this->d;
    }

    int getSearchSpace() {
        return this->searchSpace;
    }

    int getRelaxedEdges() {
        return this->relaxedEdges;
    }

    vector<int> getPath() {
        // retrace path from end to start
        vector<int> path;
        path.push_back(this->t);
        if (this->s == this->t) {
            return path;
        }
        int parentVertex = (*this->vertexParents)[this->t];
        path.push_back(parentVertex);
        while (parentVertex != this->s) {
            parentVertex = (*this->vertexParents)[parentVertex];
            path.push_back(parentVertex);
        }
        // reverse path
        std::reverse(path.begin(),path.end());

        return path;
    }

    void printVertexWeights() {
        GraphUtils::printVector(*(this->vertexWeights));
    }
  

private:
    int t;
    int s;
    vector<vector<CHQueryEdge>> graph;
    bool direction = true; // Start with the upward search
    set<pair<int, int>>* vertexSet;
    set<pair<int, int>> vertexSet_up;
    set<pair<int, int>> vertexSet_down;
    vector<int>* vertexWeights;
    vector<int> vertexWeights_up;
    vector<int> vertexWeights_down;
    vector<int>* vertexParents;
    vector<int> vertexParents_up;
    vector<int> vertexParents_down;
    vector<int>* stallingDistances;
    vector<int> stallingDistances_up;
    vector<int> stallingDistances_down;
    int weight_upward_search = 0;
    int weight_downward_search = 0;
    int d = -1; // best path weight so far (-1 : infinity)
    int searchSpace = 0;
    int relaxedEdges = 0;

    set<pair<int, int>> initVertexSet(int s) {
        set<pair<int, int>> vertexSet;
        vertexSet.insert(make_pair(0, s)); 
        return vertexSet;
    }

    vector<int> initVertexWeights(int s) {
        vector<int> vertexWeights(graph.size(), -1); // -1 corresponds to infinite weight
        vertexWeights[s] = 0;
        return vertexWeights;
    }

    vector<int> initVertexParents() {
        return vector<int>(graph.size(), -1); // -1 corresponds to no parent
    }

    bool checkStoppingCriteria(int visitedWeight) {
        // Check min queue weight
        if (this->direction) {
            weight_upward_search = visitedWeight;
        } else {
            weight_downward_search = visitedWeight;
        }

        int minQueueWeight = fmin(weight_downward_search, weight_upward_search);
        return (this->d != -1 && this->d <= minQueueWeight); // No possible better path can be found
    }

    void setDirection() {
        if (this->direction && !this->vertexSet_down.empty()) {
            // Switch down
            this->direction = false;
            this->vertexSet = &this->vertexSet_down;
            this->vertexWeights = &this->vertexWeights_down;
            this->vertexParents = &this->vertexParents_down;
            this->stallingDistances = &this->stallingDistances_down;
        } else if (!this->direction && !this->vertexSet_up.empty()) {
            // Switch up
            this->direction = true;
            this->vertexSet = &this->vertexSet_up;
            this->vertexWeights = &this->vertexWeights_up;
            this->vertexParents = &this->vertexParents_up;
            this->stallingDistances = &this->stallingDistances_up;
        }
    }

    void updateD(int visitedVertexNb) {
        int weight_up = this->vertexWeights_up[visitedVertexNb];
        int weight_down = this->vertexWeights_down[visitedVertexNb];
        if (weight_up != -1 && weight_down != -1) { // vertex reached in both direction
            if (weight_up + weight_down < this->d || this->d == -1) {
                this->d = weight_up + weight_down;
            }
        }
    }
    
};