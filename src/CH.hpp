#include <unordered_set>
using std::unordered_set;

class CHGraph {
public:
    CHGraph(vector<vector<Edge>> inputGraph) {
        this->incidenceList = vector<pair<vector<CHEdge*>, vector<CHEdge*>>>(inputGraph.size(), make_pair(vector<CHEdge*>(), vector<CHEdge*>()));
        for (int sourceVertex = 0; sourceVertex<inputGraph.size(); sourceVertex++) {
            vector<Edge> edges = inputGraph[sourceVertex];
            for (auto& edge : edges) {
                int destinationVertex = edge.getDestinationVertex();
                CHEdge* edgePtr = new CHEdge(sourceVertex, destinationVertex, edge.getWeight());
                this->incidenceList[sourceVertex].first.push_back(edgePtr);
                this->incidenceList[destinationVertex].second.push_back(edgePtr);
                this->remainingEdgesNb++;
            }
        }
        this->remainingVerticesNb = inputGraph.size();
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
                    this->remainingEdgesNb--;
                }
            }
        }
        for (auto& edgePtr : this->incidenceList[vertex].second) { // ingoing edges
            int sourceVertex = edgePtr->getSourceVertex();
            for (int edgeIndex = 0; edgeIndex<this->incidenceList[sourceVertex].first.size(); edgeIndex++) {
                CHEdge* edgePtr2 = this->incidenceList[sourceVertex].first[edgeIndex];
                if (edgePtr == edgePtr2) {
                    this->incidenceList[sourceVertex].first.erase(this->incidenceList[sourceVertex].first.begin() + edgeIndex);
                    this->remainingEdgesNb--;
                }
            }
        }
        this->remainingVerticesNb--;
    }

    void removeEdge(CHEdge* edgePtr) {
        // Detach from source
        int sourceVertex = edgePtr->getSourceVertex();
        for (int sourceEdgeIndex = 0; sourceEdgeIndex<this->incidenceList[sourceVertex].first.size(); sourceEdgeIndex++) {
            CHEdge* edgePtr2 = this->incidenceList[sourceVertex].first[sourceEdgeIndex];
            if (edgePtr == edgePtr2) {
                this->incidenceList[sourceVertex].first.erase(this->incidenceList[sourceVertex].first.begin() + sourceEdgeIndex);
            }
        }
        // Detach from destination
        int destinationVertex = edgePtr->getDestinationVertex();
        for (int destinationEdgeIndex = 0; destinationEdgeIndex<this->incidenceList[destinationVertex].second.size(); destinationEdgeIndex++) {
            CHEdge* edgePtr2 = this->incidenceList[destinationVertex].second[destinationEdgeIndex];
            if (edgePtr == edgePtr2) {
                this->incidenceList[destinationVertex].second.erase(this->incidenceList[destinationVertex].second.begin() + destinationEdgeIndex);
            }
        }
        this->remainingEdgesNb--;
    }

    bool updateEdge(int u, int v, float newWeight) {
        bool alreadyEdge = false;
        for (auto& edgePtr : this->incidenceList[u].first) { // outgoing from u
            if (edgePtr->getDestinationVertex() == v) {
                // There exists a u-v edge : it should be replaced
                alreadyEdge = true;
                edgePtr->setWeight(newWeight);
            }
        }
        return alreadyEdge;
    }

    void addEdge(CHEdge* edgePtr) {
        this->incidenceList[edgePtr->getSourceVertex()].first.push_back(edgePtr); // outgoing from u
        this->incidenceList[edgePtr->getDestinationVertex()].second.push_back(edgePtr); // ingoing to v
        this->remainingEdgesNb++;
    }

    float getAverageDegree() {
        return (float)remainingEdgesNb/(float)remainingVerticesNb;
    }

private:
    vector<pair<vector<CHEdge*>, vector<CHEdge*>>> incidenceList;
    int remainingVerticesNb = 0;
    int remainingEdgesNb = 0;
};

class CH {
/* This class implements Contracion Hierarchies preprocessing functions */
public:
    CH(vector<vector<Edge>> inputGraph) : graph(inputGraph) {
        this->inputGraph = inputGraph;
        this->n = inputGraph.size();
        this->deletedNeighbours = vector<int>(this->n, 0);
        this->vertexWeights = vector<float>(this->n, -1);
        this->vertexWeightsR = vector<float>(this->n, -1);
        this->hops = vector<int>(this->n, 0);
        this->hopsR = vector<int>(this->n, 0);
    }

    vector<vector<CHQueryEdge>> preprocess() {
        auto t1 = std::chrono::high_resolution_clock::now();
        this->buildVertexOrdering();
        CHGraph g_H = constructCH();
        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        std::cout << "CH duration : " << duration/1000000.0 << " seconds" << endl;
        vector<vector<CHQueryEdge>> g_star = convertToSearchGraph(g_H);
        return g_star;
    }

private:
    CHGraph graph;
    vector<vector<Edge>> inputGraph;
    int n;
    set<pair<float, int>> vertexOrdering;
    vector<int> vertexOrderMap;
    vector<float> vertexOrderingScores; // Used to find elements in vertexOrdering
    vector<int> deletedNeighbours;
    CHGraph g_H = graph; // CH graph
    CHGraph g_R = graph; // Remaining graph
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

    float calcPriorityScore(int vertexNb) {
        vector<int> simulResults = contractVertex(vertexNb, true);
        int addedShortcuts = simulResults[0];
        int s = simulResults[1]; // Search space
        int incidentEdges = g_R.getIngoingEdges(vertexNb).size() + g_R.getOutgoingEdges(vertexNb).size();
        int e = addedShortcuts - incidentEdges; // Edge difference
        int d = deletedNeighbours[vertexNb];// Deleted neighbours
        return 190*e + 120*d + 10*s;
    }

    CHGraph constructCH() {
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

        return g_H;
    }

    void updateDeletedNeighbours(int deletedVertex) {
        for (auto& neighbour : g_R.getNeighbours(deletedVertex)) {
            this->deletedNeighbours[neighbour]++;
        }
    }

    vector<int> contractVertex(int vertexNb, bool simulation) {
        int addedShortcuts = 0;
        int searchSpace = 0;

        vector<CHEdge> ingoingEdges = g_R.getIngoingEdges(vertexNb);
        vector<CHEdge> outgoingEdges = g_R.getOutgoingEdges(vertexNb);
        
        for (CHEdge ingoingEdge : ingoingEdges) {
            int u = ingoingEdge.getSourceVertex();
            vector<int> v_list;
            vector<float> referenceWeight_list;
            for (CHEdge outgoingEdge : outgoingEdges) {
                int v = outgoingEdge.getDestinationVertex();
                if (u != v) {
                    v_list.push_back(v);
                    referenceWeight_list.push_back(ingoingEdge.getWeight() + outgoingEdge.getWeight());
                }
            }
            if (v_list.size() > 0) {
                // Compare path weight going through removed vertex x_i with shortest path weight without x_i
                float stoppingDistance = *max_element(referenceWeight_list.begin(), referenceWeight_list.end());
                pair<vector<float>, int> dikstraResult = dijkstraCH(u, v_list, stoppingDistance, vertexNb);
                vector<float> shortest_list = dikstraResult.first;
                searchSpace += dikstraResult.second;
                for (int i = 0; i<shortest_list.size(); i++) {
                    float shortest = shortest_list[i];
                    float referenceWeight = referenceWeight_list[i];
                    int v = v_list[i];
                    if ((shortest != -1) && (shortest <= referenceWeight)) {
                        // Found witness path : should not add shortcut
                    } else {
                        // Should add shortcut
                        addedShortcuts++;
                        if (!simulation) {
                            if (g_R.updateEdge(u, v, referenceWeight)) {
                                g_H.updateEdge(u, v, referenceWeight);
                            } else {
                                CHEdge* edgePtr = new CHEdge(u, v, referenceWeight);
                                g_R.addEdge(edgePtr);
                                g_H.addEdge(edgePtr);
                            }
                            
                        }
                    }
                }
            }
        }
        return vector<int>({addedShortcuts, searchSpace});
    }

    pair<vector<float>, int> dijkstraCH(int s, vector<int> t_list, float stoppingDistance, int ignoreVertex) {
        // Init
        set<pair<float, int>> vertexSet;
        vertexSet.insert(make_pair(0.0f, s));
        this->hops = this->hopsR; 
        this->vertexWeights = this->vertexWeightsR;
        vertexWeights[s] = 0;

        vector<float> results = vector<float>(t_list.size(), -1);
        int visitedCount = 0;
        int searchSpace = 0;

        while (!vertexSet.empty()) {
            searchSpace++;
            // pop first vertex in the set
            pair<float, int> visitedVertex = *(vertexSet.begin());
            vertexSet.erase(vertexSet.begin());
            float visitedVertexWeight = visitedVertex.first;
            int visitedVertexNb = visitedVertex.second;

            // update visited t count
            for (auto& t : t_list) {
                if (visitedVertexNb == t) {
                    visitedCount++;
                }
            }
            if ((visitedCount == t_list.size()) || (visitedVertexWeight > stoppingDistance)) { // if all t's visited or if no more possible shortest path
                break;
            } else {
                if (hops[visitedVertexNb] > hopLimits[hopIndex]) continue;
                for (auto& e : g_R.getIncidenceList()[visitedVertexNb].first) {
                    if (e->getDestinationVertex() == ignoreVertex) continue;  // Ignore the vertex that will be removed in the graph
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
                        // UPDATE weight
                        vertexWeights[e->getDestinationVertex()] = neighbourNewWeight;
                        hops[e->getDestinationVertex()] = hops[visitedVertexNb] + 1;
                    }
                }
            }
        }
        // On-the-fly edge reduction
        for (int edgeIndex = 0; edgeIndex < g_R.getIncidenceList()[s].first.size(); edgeIndex++) {
            CHEdge* edge = g_R.getIncidenceList()[s].first[edgeIndex];
            float newWeight = vertexWeights[edge->getDestinationVertex()];
            if (newWeight != -1 && newWeight < edge->getWeight()) {
                // Remove edge
                g_R.removeEdge(edge);
                g_H.removeEdge(edge);

            }
        }
        // Return result
        for (int i = 0; i < t_list.size(); i++) {
            int t = t_list[i];
            results[i] = vertexWeights[t];
        }
        return make_pair(results, searchSpace);
    }

    vector<vector<CHQueryEdge>> convertToSearchGraph(CHGraph g_H) {
        vector<vector<CHQueryEdge>> g_star = vector<vector<CHQueryEdge>>(this->n, vector<CHQueryEdge>());
        
        for (int u = 0; u<n; u++) {
            vector<CHEdge*> edgePtrs = g_H.getIncidenceList()[u].first;
            for (auto& edgePtr : edgePtrs) {
                int v = edgePtr->getDestinationVertex();
                bool direction = (vertexOrderMap[u] < vertexOrderMap[v]);
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