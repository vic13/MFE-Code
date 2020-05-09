

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