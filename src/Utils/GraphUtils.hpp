class GraphUtils {
public:
    static vector<vector<TDEdge>> convertToTDGraph(vector<vector<Edge>> graph) {
        vector<vector<TDEdge>> tdGraph;
        for (auto& edges : graph) {
            vector<TDEdge> tdEdges;
            for (auto& edge : edges) {
                tdEdges.push_back(TDEdge(edge.getDestinationVertex(), TTF(edge.getWeight())));
            }
            tdGraph.push_back(tdEdges);
        }
        return tdGraph;
    }

    template <typename T>
    static void printNetwork(vector<vector<T>> graph, int nbVertices) {
        for (int a=0; a<nbVertices; a++) {
            vector<T> neighbours = graph[a];
            cout << "nb neighbours : " << neighbours.size() << endl;
            for (int b=0; b<neighbours.size(); b++) {
                cout << "weight : " << neighbours[b].getWeight() << endl;
                cout << "dest : " << neighbours[b].getDestinationVertex() << endl;
            }
        }
    }

    template <typename T>
    static int getSize(vector<vector<T>> graph) {
        int size = 0;
        for (auto& edges : graph) {
            for (auto& edge : edges) {
                size += sizeof(edge);
            }
        }
        return size;
    }

    template <typename T>
    static void printVector(std::vector<T> const &input) {
        cout << "[";
        std::copy(input.begin(), input.end(), std::ostream_iterator<int>(cout, " "));
        cout << "]" << endl;
    }

    template <typename T>
    static void printGraphProperties(vector<vector<T>> graph) {
        int nbVertices = graph.size();
        int nbEdges = getNbEdges(graph);

        cout << "------- Graph properties -------" << endl;
        cout << "#Vertices : " << nbVertices << endl;
        cout << "#Edges : " << nbEdges << endl;
        cout << "--------------------------------" << endl;
    }

    template <typename T>
    static int getNbEdges(vector<vector<T>> graph) {
        int nbEdges = 0;
        for (auto& vertexEdges : graph) {
            nbEdges += vertexEdges.size();
        }
        return nbEdges;
    }

    static vector<vector<Edge>> reverseGraph(vector<vector<Edge>>& graph) {
        vector<vector<Edge>> reversedGraph(graph.size(), vector<Edge>());
        for (int source = 0; source<graph.size(); source++) {
            for (auto& edge : graph[source]) {
                reversedGraph[edge.getDestinationVertex()].push_back(Edge(source, edge.getWeight()));
            }
        }
        return reversedGraph;
    }
};


