
class TCH : public CHTemplate<TDEdge, TCHEdge, TCHQueryEdge> {
public:
    TCH(vector<vector<TDEdge>> inputGraph) : CHTemplate<TDEdge, TCHEdge, TCHQueryEdge>(inputGraph) {}

    vector<int> contractVertex(int vertexNb, bool simulation) {
        return vector<int>({0, 0});
    }
};