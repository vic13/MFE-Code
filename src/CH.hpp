


class CHEdge : public Edge {
public:
    CHEdge(int sourceVertex, int destinationVertex, float weight): Edge(destinationVertex, weight) {
        this->sourceVertex = sourceVertex;
    }

    int getSourceVertex() {
        return this->sourceVertex;
    }

    void setWeight(float weight) {
        this->weight = weight;
    }

private:
    int sourceVertex;
};

class CHGraph {
public:
    CHGraph(vector<vector<Edge>> inputGraph) {
        this->incidenceList = vector<pair<vector<CHEdge*>, vector<CHEdge*>>>(inputGraph.size(), make_pair(vector<CHEdge*>(), vector<CHEdge*>()));
        //this->incidenceList = vector<vector<bool>>(n, vector<bool>(n, false));
        for (int sourceVertex = 0; sourceVertex<inputGraph.size(); sourceVertex++) {
            vector<Edge> edges = inputGraph[sourceVertex];
            for (auto& edge : edges) {
                int destinationVertex = edge.getDestinationVertex();
                CHEdge* edgePtr = new CHEdge(sourceVertex, destinationVertex, edge.getWeight());
                this->incidenceList[sourceVertex].first.push_back(edgePtr);
                this->incidenceList[destinationVertex].second.push_back(edgePtr);
            }
        }
    }

    vector<CHEdge> getOutgoingEdges(int vertex) {
        vector<CHEdge> result;
        for (auto& edgePtr : this->incidenceList[vertex].first) {
            result.push_back(*edgePtr);
        }
        return result;
    }

    vector<CHEdge> getIngoingEdges(int vertex) {
        vector<CHEdge> result;
        for (auto& edgePtr : this->incidenceList[vertex].second) {
            result.push_back(*edgePtr);
        }
        return result;
    }

    vector<pair<vector<CHEdge*>, vector<CHEdge*>>> getIncidenceList() {
        return this->incidenceList;
    }

    void removeVertex(int vertex) {
        /* Removes the pointers to edges incident to the given vertex */
        for (auto& edgePtr : this->incidenceList[vertex].first) { // outgoing edges
            int destinationVertex = edgePtr->getDestinationVertex();
            for (int edgeIndex = 0; edgeIndex<this->incidenceList[destinationVertex].second.size(); edgeIndex++) {
                CHEdge* edgePtr2 = this->incidenceList[destinationVertex].second[edgeIndex];
                if (edgePtr == edgePtr2) {
                    this->incidenceList[destinationVertex].second.erase(this->incidenceList[destinationVertex].second.begin() + edgeIndex);
                }
            }
        }
        for (auto& edgePtr : this->incidenceList[vertex].second) { // ingoing edges
            int sourceVertex = edgePtr->getSourceVertex();
            for (int edgeIndex = 0; edgeIndex<this->incidenceList[sourceVertex].first.size(); edgeIndex++) {
                CHEdge* edgePtr2 = this->incidenceList[sourceVertex].first[edgeIndex];
                if (edgePtr == edgePtr2) {
                    this->incidenceList[sourceVertex].first.erase(this->incidenceList[sourceVertex].first.begin() + edgeIndex);
                }
            }
        }
    }

    void updateEdge(int u, int v, float newWeight) {
        bool alreadyEdge = false;
        for (auto& edgePtr : this->incidenceList[u].first) { // outgoing from u
            if (edgePtr->getDestinationVertex() == v) {
                // There exists a u-v edge : it should be replaced
                alreadyEdge = true;
                edgePtr->setWeight(newWeight);
            }
        }
        if (!alreadyEdge) {
            // Add edge
            CHEdge* edgePtr = new CHEdge(u, v, newWeight);
            this->incidenceList[u].first.push_back(edgePtr); // outgoing from u
            this->incidenceList[v].second.push_back(edgePtr); // ingoing to v
        }
    }

private:
    vector<pair<vector<CHEdge*>, vector<CHEdge*>>> incidenceList;
};

#include "DijkstraCH.hpp"

class CH {
/* This class implements Contracion Hierarchies functions, including preprocessing and querying */
public:
    CH(vector<vector<Edge>> inputGraph) : graph(inputGraph) {
        this->inputGraph = inputGraph;
    }

    CHGraph constructCH() {
        CHGraph g_H = graph; // CH graph
        CHGraph g_R = graph; // Remaining graph

        for (int priorityVertex = 0; priorityVertex < inputGraph.size(); priorityVertex++) {
            vector<CHEdge> ingoingEdges = g_R.getIngoingEdges(priorityVertex);
            vector<CHEdge> outgoingEdges = g_R.getOutgoingEdges(priorityVertex);
            g_R.removeVertex(priorityVertex);
            for (CHEdge ingoingEdge : ingoingEdges) {
                for (CHEdge outgoingEdge : outgoingEdges) {
                    int u = ingoingEdge.getSourceVertex();
                    int v = outgoingEdge.getDestinationVertex();
                    if (u != v) {
                        // Compare path weight going through removed vertex x_i with shortest path weight without x_i
                        float referenceWeight = ingoingEdge.getWeight() + outgoingEdge.getWeight();
                        DijkstraCH dijkstra(g_R.getIncidenceList(), u, v);
                        if (dijkstra.compute() && (dijkstra.getPathWeight() <= referenceWeight)) {
                            // Found witness path : should not add shortcut
                        } else {
                            // Should add shortcut
                            g_R.updateEdge(u, v, referenceWeight);
                            g_H.updateEdge(u, v, referenceWeight);
                        }
                    }
                }
            }
        }

        return g_H;
    }

private:
    CHGraph graph;
    vector<vector<Edge>> inputGraph;
};