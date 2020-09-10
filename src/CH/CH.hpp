
class CH : public CHTemplate<Edge, CHEdge, CHQueryEdge, int> {
public:
    CH(vector<vector<Edge>> inputGraph) : CHTemplate<Edge, CHEdge, CHQueryEdge, int>(inputGraph) {}

    vector<int> contractVertex(int vertexNb, bool simulation) {
        int addedShortcuts = 0;
        int searchSpace = 0;

        vector<CHEdge> ingoingEdges = g_R.getIngoingEdges(vertexNb);
        vector<CHEdge> outgoingEdges = g_R.getOutgoingEdges(vertexNb);
        
        for (CHEdge ingoingEdge : ingoingEdges) {
            int u = ingoingEdge.getSourceVertex();
            vector<int> v_list;
            vector<int> referenceWeight_list;
            for (CHEdge outgoingEdge : outgoingEdges) {
                int v = outgoingEdge.getDestinationVertex();
                if (u != v) {
                    v_list.push_back(v);
                    referenceWeight_list.push_back(ingoingEdge.getWeight() + outgoingEdge.getWeight());
                }
            }
            if (v_list.size() > 0) {
                // Compare path weight going through removed vertex x_i with shortest path weight without x_i
                int stoppingDistance = *max_element(referenceWeight_list.begin(), referenceWeight_list.end());
                pair<vector<int>, int> dikstraResult = dijkstraCH(u, v_list, stoppingDistance, vertexNb);
                vector<int> shortest_list = dikstraResult.first;
                searchSpace += dikstraResult.second;
                for (int i = 0; i<shortest_list.size(); i++) {
                    int shortest = shortest_list[i];
                    int referenceWeight = referenceWeight_list[i];
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

    pair<vector<int>, int> dijkstraCH(int s, vector<int> t_list, int stoppingDistance, int ignoreVertex) {
        // Init
        set<pair<int, int>> vertexSet;
        vertexSet.insert(make_pair(0, s));
        this->hops = this->hopsR; 
        this->vertexWeights = this->vertexWeightsR;
        vertexWeights[s] = 0;

        vector<int> results = vector<int>(t_list.size(), -1);
        int visitedCount = 0;
        int searchSpace = 0;

        while (!vertexSet.empty()) {
            searchSpace++;
            // pop first vertex in the set
            pair<int, int> visitedVertex = *(vertexSet.begin());
            vertexSet.erase(vertexSet.begin());
            int visitedVertexWeight = visitedVertex.first;
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
                    int neighbourCurrentWeight = vertexWeights[e->getDestinationVertex()];
                    int neighbourNewWeight = visitedVertexWeight + e->getWeight();
                    if ((neighbourCurrentWeight == -1) || (neighbourNewWeight < neighbourCurrentWeight)) {    // if smaller weight was found
                        if (neighbourCurrentWeight != -1) {                                                              
                            // if current weight not infinite : vertex already in queue : DELETE before inserting the updated vertex
                            pair<int, int> currentNeighbour = make_pair(neighbourCurrentWeight, e->getDestinationVertex());
                            vertexSet.erase(vertexSet.find(currentNeighbour));
                        }
                        // INSERT in queue
                        pair<int, int> newNeighbour = make_pair(neighbourNewWeight, e->getDestinationVertex());
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
            int newWeight = vertexWeights[edge->getDestinationVertex()];
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

    vector<vector<CHQueryEdge>> convertToSearchGraph(CHGraph<Edge, CHEdge, int> g_H) {
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