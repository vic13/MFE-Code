
class BarabasiAlbertGraph {
public:
    /// Initialize the graph based on the specified number of vertices 'n' and initial number of connected vertices 'm'
    BarabasiAlbertGraph(int n, int m) {
        this->n = n;
        this->m = m;
        this->adjacencyMatrix = vector<vector<bool>>(n, vector<bool>(n, false));
        this->degrees = vector<int>(n, 0);
    }

    /// Build a scale-free graph using the Barabasi-Albert procedure, and return it as an adjacency list
    vector<vector<Edge>> build() {
        // Start with m connected vertices
        for (int i=0; i<m; i++) {
            for (int j=0; j<m; j++) {
                if (i!=j) {
                    adjacencyMatrix[i][j] = 1;
                }
            }
            degrees[i] = m-1;
        }
        degreeSum = m*(m-1);

        // Add vertices until n vertices
        for (int v1=m; v1<n; v1++) {
            // Attach m edges (proportionally to vertex degree)
            for (int edge=0; edge<m; edge++) {
                bool succeeded = false;
                while (!succeeded) {
                    succeeded = tryToAddEdge(v1);
                }
            }
        }

        return this->asAdjacencyList();
    }
    
private:
    int n; // number of vertices
    int m; // number of edges
    vector<vector<bool>> adjacencyMatrix;
    vector<int> degrees;
    int degreeSum;

    bool tryToAddEdge(int v1) {
        int r = rand() % degreeSum;
        int cumulDegree = 0;
        for (int v2 = 0; v2<=v1; v2++) {
            cumulDegree += degrees[v2];
            if (r<cumulDegree) {
                if (adjacencyMatrix[v1][v2] || v1==v2) { // Check if already linked or if two identical vertices
                    return false;
                } else {
                    addEdge(v1, v2);
                    return true;
                }
            }
        }
        exit(0); // To silence non-returning warning
    }

    void addEdge(int v1, int v2) {
        adjacencyMatrix[v1][v2] = true;
        adjacencyMatrix[v2][v1] = true;
        degrees[v1]++;
        degrees[v2]++;
        degreeSum+=2;
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


