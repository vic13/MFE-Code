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
    while (getline(ifs, s, ',')) {
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
    int seed = PARAMS_SEED;
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

TTF randomTTF() {
    vector<pair<float,float>> points;
    float max = 600;
    float x = 0;
    float y = max;
    points.push_back(make_pair(x,y));
    while (true) {
        float dx = 60*random01();
        if (x+dx > TTF::period) dx=TTF::period-x;
        float newY = max*random01();
        float dy = newY - y;
        // cout << dy/dx << endl;
        if (dy/dx < -1) { // not respect FIFO
            dy = -0.999*dx;
        }
        y += dy;
        x += dx;
        points.push_back(make_pair(x,y));
        if (x==TTF::period) break;
    }
    return TTF(points);
}

vector<vector<TDEdge>> convertToTDGraph(vector<vector<Edge>> graph) {
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

class Clock {
public:
    Clock(const char* message, bool average, int precision = 0) {
        this->message = message;
        this->precision = precision;
        this->average = average;
        this->t1 = std::chrono::high_resolution_clock::now();
    }

    void start() {
        this->t1 = std::chrono::high_resolution_clock::now();
    }

    void lap(bool display = false) {
        auto t2 = std::chrono::high_resolution_clock::now();
        this->duration += std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        this->nbLaps++;
        if (display) {
            this->display();
        }
    }

    void display() {
        if (this->average) duration /= nbLaps;
        if (precision == 0) cout << message << " : " << duration/1000000.0 << " s" << endl;
        if (precision == 1) cout << message << " : " << duration/1000.0 << " ms" << endl;
        if (precision == 2) cout << message << " : " << duration << " µs" << endl;
    }
private:
    std::chrono::steady_clock::time_point t1;
    long duration = 0;
    const char* message;
    int precision;
    bool average;
    int nbLaps = 0;
};