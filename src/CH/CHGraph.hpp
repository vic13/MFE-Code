#include <unordered_set>
using std::unordered_set;

template <class T_Edge, class T_CHEdge, class T_weight>
class CHGraph {
public:
    /// Build a CHGraph from the input graph. CHGraph is the graph structure used by the remaining graph and the Contraction Hierarchy during CH preprocessing
    CHGraph(vector<vector<T_Edge>> inputGraph) {
        this->incidenceList = vector<pair<vector<T_CHEdge*>, vector<T_CHEdge*>>>(inputGraph.size(), make_pair(vector<T_CHEdge*>(), vector<T_CHEdge*>()));
        for (int sourceVertex = 0; sourceVertex<inputGraph.size(); sourceVertex++) {
            vector<T_Edge> edges = inputGraph[sourceVertex];
            for (auto& edge : edges) {
                int destinationVertex = edge.getDestinationVertex();
                T_CHEdge* edgePtr = new T_CHEdge(sourceVertex, destinationVertex, edge.getWeight());
                this->incidenceList[sourceVertex].first.push_back(edgePtr);
                this->incidenceList[destinationVertex].second.push_back(edgePtr);
                this->remainingEdgesNb++;
            }
        }
        this->remainingVerticesNb = inputGraph.size();
    }

    /// Return the edges leaving the specified vertex
    vector<T_CHEdge> getOutgoingEdges(int vertex) {
        vector<T_CHEdge> result;
        for (auto& edgePtr : this->incidenceList[vertex].first) {
            result.push_back(*edgePtr);
        }
        return result;
    }

    /// Return the edges arriving to the specified vertex
    vector<T_CHEdge> getIngoingEdges(int vertex) {
        vector<T_CHEdge> result;
        for (auto& edgePtr : this->incidenceList[vertex].second) {
            result.push_back(*edgePtr);
        }
        return result;
    }

    /// Return the neighbour vertices of the specified vertex
    unordered_set<int> getNeighbours(int vertex) {
        unordered_set<int> neighbours;
        for (auto& edgePtr : this->incidenceList[vertex].first) {
            neighbours.insert(edgePtr->getDestinationVertex());
        }
        for (auto& edgePtr : this->incidenceList[vertex].second) {
            neighbours.insert(edgePtr->getSourceVertex());
        }
        return neighbours;
    }

    vector<pair<vector<T_CHEdge*>, vector<T_CHEdge*>>>& getIncidenceList() {
        return this->incidenceList;
    }

    /// Removes the pointers to edges incident to the given vertex
    void removeVertex(int vertex) {
        for (auto& edgePtr : this->incidenceList[vertex].first) { // outgoing edges
            int destinationVertex = edgePtr->getDestinationVertex();
            for (int edgeIndex = 0; edgeIndex<this->incidenceList[destinationVertex].second.size(); edgeIndex++) {
                T_CHEdge* edgePtr2 = this->incidenceList[destinationVertex].second[edgeIndex];
                if (edgePtr == edgePtr2) {
                    this->incidenceList[destinationVertex].second.erase(this->incidenceList[destinationVertex].second.begin() + edgeIndex);
                    this->remainingEdgesNb--;
                }
            }
        }
        for (auto& edgePtr : this->incidenceList[vertex].second) { // ingoing edges
            int sourceVertex = edgePtr->getSourceVertex();
            for (int edgeIndex = 0; edgeIndex<this->incidenceList[sourceVertex].first.size(); edgeIndex++) {
                T_CHEdge* edgePtr2 = this->incidenceList[sourceVertex].first[edgeIndex];
                if (edgePtr == edgePtr2) {
                    this->incidenceList[sourceVertex].first.erase(this->incidenceList[sourceVertex].first.begin() + edgeIndex);
                    this->remainingEdgesNb--;
                }
            }
        }
        this->remainingVerticesNb--;
    }

    /// Remove the specified edge
    void removeEdge(T_CHEdge* edgePtr) {
        // Detach from source
        int sourceVertex = edgePtr->getSourceVertex();
        for (int sourceEdgeIndex = 0; sourceEdgeIndex<this->incidenceList[sourceVertex].first.size(); sourceEdgeIndex++) {
            T_CHEdge* edgePtr2 = this->incidenceList[sourceVertex].first[sourceEdgeIndex];
            if (edgePtr == edgePtr2) {
                this->incidenceList[sourceVertex].first.erase(this->incidenceList[sourceVertex].first.begin() + sourceEdgeIndex);
            }
        }
        // Detach from destination
        int destinationVertex = edgePtr->getDestinationVertex();
        for (int destinationEdgeIndex = 0; destinationEdgeIndex<this->incidenceList[destinationVertex].second.size(); destinationEdgeIndex++) {
            T_CHEdge* edgePtr2 = this->incidenceList[destinationVertex].second[destinationEdgeIndex];
            if (edgePtr == edgePtr2) {
                this->incidenceList[destinationVertex].second.erase(this->incidenceList[destinationVertex].second.begin() + destinationEdgeIndex);
            }
        }
        this->remainingEdgesNb--;
    }

    /// Update the weight of the specified edge if it exists, return 'false' otherwise
    bool updateEdge(int u, int v, T_weight newWeight) {
        bool alreadyEdge = false;
        for (auto& edgePtr : this->incidenceList[u].first) { // outgoing from u
            if (edgePtr->getDestinationVertex() == v) {
                // There exists a u-v edge : it should be replaced
                alreadyEdge = true;
                edgePtr->setMinWeight(newWeight);
            }
        }
        return alreadyEdge;
    }

    /// Add the specified edge by adding a reference at its source vertex and destination vertex
    void addEdge(T_CHEdge* edgePtr) {
        this->incidenceList[edgePtr->getSourceVertex()].first.push_back(edgePtr); // outgoing from u
        this->incidenceList[edgePtr->getDestinationVertex()].second.push_back(edgePtr); // ingoing to v
        this->remainingEdgesNb++;
    }

    /// Return the average degree of the graph
    float getAverageDegree() {
        return (float)remainingEdgesNb/(float)remainingVerticesNb;
    }

private:
    vector<pair<vector<T_CHEdge*>, vector<T_CHEdge*>>> incidenceList;
    int remainingVerticesNb = 0;
    int remainingEdgesNb = 0;
};