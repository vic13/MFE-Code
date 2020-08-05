#include <fstream>
#include <sstream>
using std::stringstream;

void writeGraphToFile(const char* filePath, vector<vector<CHQueryEdge>> graph) {
    std::ofstream ofs(filePath);
    for (auto& vertexEdges : graph) {
        for (auto& edge : vertexEdges) {
            ofs << edge.getDestinationVertex() << "," << edge.getWeight() << "," << edge.getDirection() << ",";
        }
        ofs << ";,";
    }
}

vector<vector<CHQueryEdge>> readGraphFromFile(const char* filePath) {
    std::ifstream ifs(filePath);
    vector<vector<CHQueryEdge>> graph;
    vector<CHQueryEdge> edges;
    string s;
    int dest = 0;
    float w = 0;
    bool direction = 0;
    int state = 0;
    while(getline(ifs, s, ',')) {
        stringstream ss(stringstream::in | stringstream::out);
        ss << s;
        if (s==";") {
            graph.push_back(edges);
            edges.clear();
        } else {
            if (state == 0) {
                ss >> dest;
                state = 1;
            } else if (state == 1) {
                ss >> w;
                state = 2;
            } else if (state == 2) {
                ss >> direction;
                state = 0;
                edges.push_back(CHQueryEdge(dest, w, direction));
            }
        }
    }
    return graph;
}

void initRandom() {
    int seed = Parameters::seed;
    if (seed == -1) {
        seed = time(NULL);
    }
    srand(seed);
    cout << "Seed : " << seed << endl;
}

float random01() {
    return ((float) rand() / (RAND_MAX));
}

int randomInt(int moduloValue) {
    return rand() % moduloValue;
}

template <typename T>
void printNetwork(vector<vector<T>> graph, int nbVertices) {
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
void print_vector(std::vector<T> const &input) {
    cout << "[";
	std::copy(input.begin(), input.end(), std::ostream_iterator<float>(cout, " "));
    cout << "]" << endl;
}

template <typename T>
void print_graph_properties(vector<vector<T>> graph) {
    int nbVertices = graph.size();
    int nbEdges = 0;

    for (auto& vertexEdges : graph) {
        nbEdges += vertexEdges.size();
    }

    cout << "------- Graph properties -------" << endl;
    cout << "#Vertices : " << nbVertices << endl;
    cout << "#Edges : " << nbEdges << endl;
    cout << "--------------------------------" << endl;
}

vector<vector<Edge>> reverseGraph(vector<vector<Edge>>& graph) {
    vector<vector<Edge>> reversedGraph(graph.size(), vector<Edge>());
    for (int source = 0; source<graph.size(); source++) {
        for (auto& edge : graph[source]) {
            reversedGraph[edge.getDestinationVertex()].push_back(Edge(source, edge.getWeight()));
        }
    }
    return reversedGraph;
}