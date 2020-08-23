
#include <set>
using std::set;

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
            pair<float, int> visitedVertex = *(vertexSet.begin());
            vertexSet.erase(vertexSet.begin());
            float visitedVertexWeight = visitedVertex.first;
            int visitedVertexNb = visitedVertex.second;

            if (visitedVertexNb == t) {
                // finished
                return true;
            } else {
                for (auto& e : graph[visitedVertexNb]) {
                    float neighbourCurrentWeight = vertexWeights[e.getDestinationVertex()];
                    float neighbourNewWeight = visitedVertexWeight + getEdgeWeight(e, visitedVertexWeight);
                    if ((neighbourCurrentWeight == -1.0f) || (neighbourNewWeight < neighbourCurrentWeight)) {    // if smaller weight was found
                        if (neighbourCurrentWeight != -1.0f) {                                                              
                            // if current weight not infinite : vertex already in queue : DELETE before inserting the updated vertex
                            pair<float, int> currentNeighbour = make_pair(neighbourCurrentWeight, e.getDestinationVertex());
                            vertexSet.erase(vertexSet.find(currentNeighbour));
                        }
                        // INSERT in queue
                        pair<float, int> newNeighbour = make_pair(neighbourNewWeight, e.getDestinationVertex());
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

    float getPathWeight() {
        return this->vertexWeights[this->t];
    }

    vector<float> getWeights() {
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
        print_vector(this->vertexWeights);
    }

    virtual float getEdgeWeight(T e, float visitedVertexWeight) = 0;
  
private:
    int t;
    int s;
    vector<vector<T>> graph;
    set<pair<float, int>> vertexSet;
    vector<float> vertexWeights;
    vector<int> vertexParents;
    int searchSpace = 0;

    set<pair<float, int>> initVertexSet(int s) {
        set<pair<float, int>> vertexSet;
        vertexSet.insert(make_pair(0.0f, s)); 
        return vertexSet;
    }

    vector<float> initVertexWeights(int s) {
        vector<float> vertexWeights(graph.size(), -1); // -1 corresponds to infinite weight
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

    float getEdgeWeight(Edge e, float visitedVertexWeight) {
        return e.getWeight();
    }
};

class DijkstraTD : public DijkstraTemplate<TDEdge> {
public:
    DijkstraTD(vector<vector<TDEdge>>& graph, int s, int t, float startingTime): DijkstraTemplate<TDEdge>(graph, s, t) {
        this->startingTime = startingTime;
    }

    float getEdgeWeight(TDEdge e, float visitedVertexWeight) {
        return e.evaluate(startingTime + visitedVertexWeight);
    }

private:
    float startingTime;
};