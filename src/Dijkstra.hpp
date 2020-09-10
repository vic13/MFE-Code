

template <class T>
class DijkstraTemplate {
public:
    DijkstraTemplate(vector<vector<T>>& graph, int s, int t) {
        this->graph = graph;
        this->t = t;
        this->s = s;
        this->vertexSet = initVertexSet(s);
        this->vertexWeights = initVertexWeights(s);
        this->vertexParents = initVertexParents();
    }

    bool compute() {
        while (!vertexSet.empty()) {
            searchSpace++;
            // pop first vertex in the set
            pair<int, int> visitedVertex = *(vertexSet.begin());
            vertexSet.erase(vertexSet.begin());
            int visitedVertexWeight = visitedVertex.first;
            int visitedVertexNb = visitedVertex.second;

            if (visitedVertexNb == t) {
                // finished
                return true;
            } else {
                for (auto& e : graph[visitedVertexNb]) {
                    if (!relaxingCondition(e)) continue;
                    int neighbourCurrentWeight = vertexWeights[e.getDestinationVertex()];
                    int neighbourNewWeight = visitedVertexWeight + getEdgeWeight(e, visitedVertexWeight);
                    if ((neighbourCurrentWeight == -1) || (neighbourNewWeight < neighbourCurrentWeight)) {    // if smaller weight was found
                        if (neighbourCurrentWeight != -1) {                                                              
                            // if current weight not infinite : vertex already in queue : DELETE before inserting the updated vertex
                            pair<int, int> currentNeighbour = make_pair(neighbourCurrentWeight, e.getDestinationVertex());
                            vertexSet.erase(vertexSet.find(currentNeighbour));
                        }
                        // INSERT in queue
                        pair<int, int> newNeighbour = make_pair(neighbourNewWeight, e.getDestinationVertex());
                        vertexSet.insert(newNeighbour);
                        // UPDATE weight + parent
                        vertexWeights[e.getDestinationVertex()] = neighbourNewWeight;
                        vertexParents[e.getDestinationVertex()] = visitedVertexNb;
                    }
                }
            }
        }
        // No solution
        return false;
    }

    int getPathWeight() {
        return this->vertexWeights[this->t];
    }

    vector<int> getWeights() {
        return this->vertexWeights;
    }

    int getSearchSpace() {
        return this->searchSpace;
    }

    vector<int> getPath() {
        // retrace path from end to start
        vector<int> path;
        path.push_back(this->t);
        if (this->s == this->t) {
            return path;
        }
        int parentVertex = this->vertexParents[this->t];
        path.push_back(parentVertex);
        while (parentVertex != this->s) {
            parentVertex = this->vertexParents[parentVertex];
            path.push_back(parentVertex);
        }
        // reverse path
        std::reverse(path.begin(),path.end());

        return path;
    }

    void printVertexWeights() {
        GraphUtils::printVector(this->vertexWeights);
    }

    virtual int getEdgeWeight(T e, int visitedVertexWeight) = 0;
    virtual bool relaxingCondition(T e) = 0;
  
protected:
    int t;
    int s;
    vector<vector<T>> graph;
    set<pair<int, int>> vertexSet;
    vector<int> vertexWeights;
    vector<int> vertexParents;
    int searchSpace = 0;

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
};

class Dijkstra : public DijkstraTemplate<Edge> {
public:
    Dijkstra(vector<vector<Edge>>& graph, int s, int t): DijkstraTemplate<Edge>(graph, s, t) {}

    int getEdgeWeight(Edge e, int visitedVertexWeight) {
        return e.getWeight();
    }

    bool relaxingCondition(Edge e) {
        return true;
    }
};

class DijkstraTD : public DijkstraTemplate<TDEdge> {
public:
    DijkstraTD(vector<vector<TDEdge>>& graph, int s, int t, int startingTime): DijkstraTemplate<TDEdge>(graph, s, t) {
        this->startingTime = startingTime;
    }

    int getEdgeWeight(TDEdge e, int visitedVertexWeight) {
        return e.evaluate(startingTime + visitedVertexWeight);
    }

    bool relaxingCondition(TDEdge e) {
        return true;
    }

private:
    int startingTime;
};

