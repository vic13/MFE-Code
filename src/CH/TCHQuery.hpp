
class DijkstraTCH : public DijkstraTemplate<TCHQueryEdge> {
public:
    DijkstraTCH(vector<vector<TCHQueryEdge>>& graph, int s, int t, int startingTime): DijkstraTemplate<TCHQueryEdge>(graph, s, t) {
        this->startingTime = startingTime;
        this->vertexReached = vector<bool>(graph.size(), false);
    }

    int getEdgeWeight(TCHQueryEdge& e, int scannedVertexWeight) {
        return e.evaluate(startingTime + scannedVertexWeight);
    }

    bool relaxingCondition(TCHQueryEdge& e) {
        return (e.getDirection() || e.isReachable()); // Up or reachable down
    }

    /// Implement first phase of TCH query : mark all edges that are reachable from destination vertex 't'
    int markReachable() {
        vertexReached[t] = true;
        queue<int> q({t});
        int searchSpace = 0;
        while (!q.empty()) {
            int x = q.front();
            q.pop();
            for (auto& edge : graph[x]) {
                if (edge.isReversed()) {
                    // Mark reachable
                    int v = edge.getDestinationVertex();
                    for (auto& analogousEdge : graph[v]) {
                        if (analogousEdge.getDestinationVertex() == x) {
                            analogousEdge.markReachable();
                            break;
                        }
                    }
                    if (!vertexReached[v]) {
                        vertexReached[v] = true;
                        q.push(v);
                        searchSpace++;
                    }
                }
            }
        }
        return searchSpace;
    }

private:
    int startingTime;
    vector<bool> vertexReached;
};