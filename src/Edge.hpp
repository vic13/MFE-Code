

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

    void setWeight(float weight) {
        this->weight = weight;
    }

protected:
    int destinationVertex;
    float weight;
};

class CHEdge : public Edge {
public:
    CHEdge(int sourceVertex, int destinationVertex, float weight): Edge(destinationVertex, weight) {
        this->sourceVertex = sourceVertex;
    }

    int getSourceVertex() {
        return this->sourceVertex;
    }

private:
    int sourceVertex;
};

class CHQueryEdge : public Edge {
public:
    CHQueryEdge(int destinationVertex, float weight, bool direction): Edge(destinationVertex, weight) {
        this->direction = direction;
    }

    bool isSameDirection(bool direction) {
        return (this->direction == direction);
    }

private:
    bool direction; // true -> up; false -> down
};