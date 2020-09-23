
class ErdosRenyi {
public:
    /// Initialize the graph based on the specified number of vertices 'n' and number of edges 'k'
    ErdosRenyi(int n, int k) {
        this->n = n;
        this->k = k;
        this->adjacencyMatrix = vector<vector<bool>>(n, vector<bool>(n, false));
        this->degrees = vector<int>(n, 0);
    }

    /// Build a random graph using the Erdos-Renyi procedure, and return it as an adjacency list
    vector<vector<Edge>> build() {
        // Add edges until k edges
        int numberOfEdges = 0;
        while (numberOfEdges < this->k) {
            bool succeeded = false;
            while (!succeeded) {
                succeeded = tryToAddEdge();
            }
            numberOfEdges++;
        }

        return this->asAdjacencyList();
    }

private:
    int n;
    int k;
    vector<vector<bool>> adjacencyMatrix;
    vector<int> degrees;

    bool tryToAddEdge() {
        // Choose 2 vertices randomly
        int v1 = rand() % n;
        int v2 = rand() % n;
        // Check if already linked or if two identical vertices
        if (v1 == v2 || adjacencyMatrix[v1][v2]) {  
            return false;
        } else {
            addEdge(v1, v2);
            return true;
        }
    }

    void addEdge(int v1, int v2) {
        adjacencyMatrix[v1][v2] = true;
        adjacencyMatrix[v2][v1] = true;
        degrees[v1]++;
        degrees[v2]++;
    }

    /// Convert the adjacency matrix to an adjacency list
    vector<vector<Edge>> asAdjacencyList() {
        vector<vector<Edge>> adjacencyList(n, vector<Edge>());
        for (int vertex = 0; vertex < n; vertex++) {
            vector<bool> neighbours = adjacencyMatrix[vertex];
            for (int neighbour = 0; neighbour < n; neighbour++) {
                if (neighbours[neighbour]) {
                    adjacencyList[vertex].push_back(Edge(neighbour, 1));
                }
            }
        }
        return adjacencyList;
    }
};
