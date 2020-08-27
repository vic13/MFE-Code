
class DijkstraTCH : public DijkstraTemplate<TCHQueryEdge> {
public:
    DijkstraTCH(vector<vector<TCHQueryEdge>>& graph, int s, int t, float startingTime): DijkstraTemplate<TCHQueryEdge>(graph, s, t) {
        this->startingTime = startingTime;
    }

    float getEdgeWeight(TCHQueryEdge e, float visitedVertexWeight) {
        return e.evaluate(startingTime + visitedVertexWeight);
    }

    bool relaxingCondition(TCHQueryEdge e) {
        return (e.getDirection() || e.isReachable()); // Up or reachable down
    }

    void markReachable() {
        vector<bool> vertexReached = vector<bool>(graph.size(), false);
        vertexReached[t] = true;
        queue<int> q({t});
        int i = 0;
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
                        i++;
                    }
                }
            }
        }
        cout << "Backward search space : " << i << endl;
    }

private:
    float startingTime;
};