

class TestGraph {
public:
    TestGraph() {

    }

    vector<vector<Edge>> build() {
        vector<vector<Edge>> adjacencyList(6, vector<Edge>());
        adjacencyList[0].push_back(Edge(1,7));
        adjacencyList[0].push_back(Edge(2,2));
        adjacencyList[1].push_back(Edge(0,7));
        adjacencyList[1].push_back(Edge(2,3));
        adjacencyList[1].push_back(Edge(3,4));
        adjacencyList[2].push_back(Edge(0,2));
        adjacencyList[2].push_back(Edge(1,3));
        adjacencyList[2].push_back(Edge(3,4));
        adjacencyList[2].push_back(Edge(4,1));
        adjacencyList[3].push_back(Edge(1,4));
        adjacencyList[3].push_back(Edge(2,4));
        adjacencyList[3].push_back(Edge(5,5));
        adjacencyList[4].push_back(Edge(2,1));
        adjacencyList[4].push_back(Edge(5,3));
        adjacencyList[5].push_back(Edge(3,5));
        adjacencyList[5].push_back(Edge(4,3));
        return adjacencyList;
    }
};