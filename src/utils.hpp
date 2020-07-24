

void initRandom(int seed = time(NULL)) {
    srand(seed);
}

float random01() {
    return ((float) rand() / (RAND_MAX));
}

int randomInt(int moduloValue) {
    return rand() % moduloValue;
}

void printNetwork(vector<vector<Edge>> graph) {
    for (int a=0; a<100; a++) {
        vector<Edge> neighbours = graph[a];
        cout << "nb neighbours : " << neighbours.size() << endl;
        for (int b=0; b<neighbours.size(); b++) {
            cout << "weight : " << neighbours[b].getWeight() << endl;
            cout << "dest : " << neighbours[b].getDestinationVertex() << endl;
        }
    }
}

void print_vector(std::vector<float> const &input) {
    cout << "[";
	std::copy(input.begin(), input.end(), std::ostream_iterator<float>(cout, " "));
    cout << "]" << endl;
}

void print_vector(std::vector<int> const &input) {
    cout << "[";
	std::copy(input.begin(), input.end(), std::ostream_iterator<int>(cout, " "));
    cout << "]" << endl;
}

void print_graph_properties(vector<vector<CHQueryEdge>> graph) {
    int nbVertices = graph.size();
    int nbEdges = 0;

    for (auto& vertexEdges : graph) {
        nbEdges += vertexEdges.size();
    }

    cout << "\nGraph properties : " << endl;
    cout << "#Vertices : " << nbVertices << endl;
    cout << "#Edges : " << nbEdges << endl;
}

void print_graph_properties(vector<vector<Edge>> graph) {
    int nbVertices = graph.size();
    int nbEdges = 0;

    for (auto& vertexEdges : graph) {
        nbEdges += vertexEdges.size();
    }

    cout << "\nGraph properties : " << endl;
    cout << "#Vertices : " << nbVertices << endl;
    cout << "#Edges : " << nbEdges << endl;
}