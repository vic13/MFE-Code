

class Edge {
public:
    
    Edge(int destinationNode, float weight) {
        this->destinationNode = destinationNode;
        this->weight = weight;
    }

    float getWeight() {
        return this->weight;
    }

    int getDestinationNode() {
        return this->destinationNode;
    }

private:
    int destinationNode;
    float weight;
};