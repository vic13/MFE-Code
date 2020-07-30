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
        this->deletedNeighbours = vector<int>(inputGraph.size(), 0);
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
    set<pair<float, int>> vertexOrdering;
    vector<int> vertexOrderMap;
    vector<float> vertexOrderingScores; // Used to find elements in vertexOrdering
    vector<int> deletedNeighbours;
    CHGraph g_H = graph; // CH graph
    CHGraph g_R = graph; // Remaining graph

    void buildVertexOrdering() {
        for (int vertexNb = 0; vertexNb < inputGraph.size(); vertexNb++) {
            float priorityScore = calcPriorityScore(vertexNb);
            vertexOrderingScores.push_back(priorityScore);
            vertexOrdering.insert(make_pair(priorityScore, vertexNb));
        }
        this->vertexOrderMap = vector<int>(inputGraph.size(), -1);
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
        //cout << s << endl;
        return 190*e + 120*d + 10*s;
    }

    CHGraph constructCH() {
        int order = 0;
        while (!vertexOrdering.empty()) {
            int priorityVertex = (*(vertexOrdering.begin())).second;
            // Lazy update : update ordering, and sample another vertex if it is no longer on top of the queue
            if (!updateOrdering(priorityVertex)) {
                cout << "continue" << endl;
                continue;
            }
            vertexOrdering.erase(vertexOrdering.begin());
            contractVertex(priorityVertex, false);
            updateDeletedNeighbours(priorityVertex); // Priority term
            g_R.removeVertex(priorityVertex);
            vertexOrderMap[priorityVertex] = order;
            cout << order << endl;
            order++;
            // Neighbours update
            unordered_set<int> neighbours = g_R.getNeighbours(priorityVertex);
            for (auto& neighbour : neighbours) {   
                updateOrdering(neighbour);
            }
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
                            g_R.updateEdge(u, v, referenceWeight);
                            g_H.updateEdge(u, v, referenceWeight);
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
        vector<float> vertexWeights(g_R.getIncidenceList().size(), -1); // -1 corresponds to infinite weight
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
                vector<CHEdge*> edges = g_R.getIncidenceList()[visitedVertexNb].first;
                for (auto& e : edges) {
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
                    }
                }
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
        int n = g_H.getIncidenceList().size();
        vector<vector<CHQueryEdge>> g_star = vector<vector<CHQueryEdge>>(n, vector<CHQueryEdge>());
        
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