

class Edge {
public:
    
    Edge(int destinationVertex, float weight) {
        this->destinationVertex = destinationVertex;
        this->weight = weight;
    }

    float getWeight() {
        return this->weight;
    }

    int getDestinationVertex() {
        return this->destinationVertex;
    }

private:
    int destinationVertex;
    float weight;
};