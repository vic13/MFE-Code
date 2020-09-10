
class TCH : public CHTemplate<TDEdge, TCHEdge, TCHQueryEdge, TTF> {
public:
    TCH(vector<vector<TDEdge>> inputGraph) : CHTemplate<TDEdge, TCHEdge, TCHQueryEdge, TTF>(inputGraph) {}

    vector<int> contractVertex(int vertexNb, bool simulation) {
        int addedShortcuts = 0;
        int searchSpace = 0;

        vector<TCHEdge> ingoingEdges = g_R.getIngoingEdges(vertexNb);
        vector<TCHEdge> outgoingEdges = g_R.getOutgoingEdges(vertexNb);
        
        for (TCHEdge ingoingEdge : ingoingEdges) {
            int u = ingoingEdge.getSourceVertex();
            vector<int> v_list;
            vector<int> referenceWeight_list;
            vector<TTF> ttf_list;
            for (TCHEdge outgoingEdge : outgoingEdges) {
                int v = outgoingEdge.getDestinationVertex();
                if (u != v) {
                    v_list.push_back(v);
                    referenceWeight_list.push_back(ingoingEdge.getMinima() + outgoingEdge.getMinima());
                    ttf_list.push_back(TTF::chaining(ingoingEdge.getWeight(), outgoingEdge.getWeight()));
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
                    TTF ttf = ttf_list[i];
                    int v = v_list[i];
                    if ((shortest != -1) && (shortest <= referenceWeight)) {
                        // Found witness path : should not add shortcut
                    } else {
                        // Should add shortcut
                        addedShortcuts++;
                        if (!simulation) {
                            if (g_R.updateEdge(u, v, ttf)) {
                                g_H.updateEdge(u, v, ttf);
                            } else {
                                TCHEdge* edgePtr = new TCHEdge(u, v, ttf);
                                g_R.addEdge(edgePtr);
                                g_H.addEdge(edgePtr);
                            }
                        }
                    }
                }
            }
        }
        // if (!simulation) cout << addedShortcuts << endl;
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
                    int neighbourNewWeight = visitedVertexWeight + e->getMaxima();
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
        
        // Return result
        for (int i = 0; i < t_list.size(); i++) {
            int t = t_list[i];
            results[i] = vertexWeights[t];
        }
        return make_pair(results, searchSpace);
    }

    vector<vector<TCHQueryEdge>> convertToSearchGraph(CHGraph<TDEdge, TCHEdge, TTF> g_H) {
        vector<vector<TCHQueryEdge>> g_star = vector<vector<TCHQueryEdge>>(this->n, vector<TCHQueryEdge>());
        for (int u = 0; u<n; u++) {
            vector<TCHEdge*> edgePtrs = g_H.getIncidenceList()[u].first;
            for (auto& edgePtr : edgePtrs) {
                int v = edgePtr->getDestinationVertex();
                bool direction = (vertexOrderMap[u] < vertexOrderMap[v]);
                g_star[u].push_back(TCHQueryEdge(v, edgePtr->getWeight(), direction));
                if (!direction) g_star[v].push_back(TCHQueryEdge(u, edgePtr->getWeight(), direction, true));
            }
        }
        return g_star;
    }
};