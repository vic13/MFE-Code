

class RoadNetwork {
    int n;
    int e;
    vector<vector<float>> adjacencyMatrix;
    
    RoadNetwork(int n, int e) {
        this->n = n;
        this->e = e;
        this->adjacencyMatrix = vector<vector<float>>(n, vector<float>(n, -1.0f)); // -1 corresponds to no connection (or infinite weight)
    }

    vector<vector<Edge>> build() {
        for (node=0; node<n; node++) {                                              // generate n nodes with random positions between 0 and 1 in x and y
            
        }
    }

};