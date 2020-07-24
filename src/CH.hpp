
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

    vector<pair<vector<CHEdge*>, vector<CHEdge*>>>& getIncidenceList() {
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

class CH {
/* This class implements Contracion Hierarchies functions, including preprocessing and querying */
public:
    CH(vector<vector<Edge>> inputGraph) : graph(inputGraph) {
        this->inputGraph = inputGraph;
    }

    vector<vector<CHQueryEdge>> preprocess() {
        cout << "a" << endl;
        CHGraph g_H = constructCH();
        cout << "b" << endl;
        vector<vector<CHQueryEdge>> g_star = convertToSearchGraph(g_H);
        cout << "c" << endl;
        return g_star;
    }

private:
    CHGraph graph;
    vector<vector<Edge>> inputGraph;

    CHGraph constructCH() {
        CHGraph g_H = graph; // CH graph
        CHGraph g_R = graph; // Remaining graph

        for (int priorityVertex = 0; priorityVertex < inputGraph.size(); priorityVertex++) {
            cout << priorityVertex << endl;
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
                        float shortest = dijkstraCH(g_R.getIncidenceList(), u, v);
                        if ((shortest != -1) && (shortest <= referenceWeight)) {
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

    float dijkstraCH( vector<pair<vector<CHEdge*>, vector<CHEdge*>>>& graph, int s, int t) {
        // Init
        set<pair<float, int>> vertexSet;
        vertexSet.insert(make_pair(0.0f, s));
        vector<float> vertexWeights(graph.size(), -1); // -1 corresponds to infinite weight
        vertexWeights[s] = 0;

        while (!vertexSet.empty()) {
            // pop first vertex in the set
            pair<float, int> visitedVertex = *(vertexSet.begin());
            vertexSet.erase(vertexSet.begin());
            float visitedVertexWeight = visitedVertex.first;
            int visitedVertexNb = visitedVertex.second;

            if (visitedVertexNb == t) {
                // finished
                return vertexWeights[t];
            } else {
                vector<CHEdge*> edges = graph[visitedVertexNb].first;
                for (auto& e : edges) {
                    float neighbourCurrentWeight = vertexWeights[e->getDestinationVertex()];
                    float neighbourNewWeight = visitedVertexWeight + e->getWeight();
                    if ((neighbourCurrentWeight == -1.0f) || (neighbourNewWeight < neighbourCurrentWeight)) {    // if smaller weight was found
                        if (neighbourCurrentWeight != -1.0f) {                                                              
                            // if current weight not infinite : vertex already in queue : DELETE before inserting the updated vertex
                            pair<float, int> currentNeighbour = make_pair(neighbourCurrentWeight, e->getDestinationVertex());
                            vertexSet.erase(vertexSet.find(currentNeighbour));
                        }
                        // INSERT in queue
                        pair<float, int> newNeighbour = make_pair(neighbourNewWeight, e->getDestinationVertex());
                        vertexSet.insert(newNeighbour);
                        // UPDATE weight + parent
                        vertexWeights[e->getDestinationVertex()] = neighbourNewWeight;
                    }
                }
            }
        }
        // No solution
        return vertexWeights[t];
    }

    vector<vector<CHQueryEdge>> convertToSearchGraph(CHGraph g_H) {
        int n = g_H.getIncidenceList().size();
        vector<vector<CHQueryEdge>> g_star = vector<vector<CHQueryEdge>>(n, vector<CHQueryEdge>());
        
        for (int u = 0; u<n; u++) {
            vector<CHEdge*> edgePtrs = g_H.getIncidenceList()[u].first;
            for (auto& edgePtr : edgePtrs) {
                int v = edgePtr->getDestinationVertex();
                bool direction = (u < v);
                if (direction) {
                    g_star[u].push_back(CHQueryEdge(v, edgePtr->getWeight(), direction));
                } else {
                    g_star[v].push_back(CHQueryEdge(u, edgePtr->getWeight(), direction));
                }
                
            }
        }

        return g_star;
    }
};