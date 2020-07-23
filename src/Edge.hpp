

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

    void setWeight(float weight) {
        this->weight = weight;
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