
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

    bool getDirection() {
        return this->direction;
    }

private:
    bool direction; // true -> up; false -> down
};

class TDEdge {
public:
    TDEdge(int destinationVertex, TTF ttf) : ttf(ttf) {
        this->destinationVertex = destinationVertex;
    }

    TTF getWeight() {
        return this->ttf;
    }

    float evaluate(float t=0) {
        return this->ttf.evaluate(t);
    }

    int getDestinationVertex() {
        return this->destinationVertex;
    }

    void setWeight(TTF ttf) {
        this->ttf = ttf;
    }

protected:
    int destinationVertex;
    TTF ttf;
};

class TCHEdge : public TDEdge {
public:
    TCHEdge(int sourceVertex, int destinationVertex, TTF ttf): TDEdge(destinationVertex, ttf) {
        this->sourceVertex = sourceVertex;
    }

    int getSourceVertex() {
        return this->sourceVertex;
    }

private:
    int sourceVertex;
};

class TCHQueryEdge : public TDEdge {
public:
    TCHQueryEdge(int destinationVertex, TTF ttf, bool direction): TDEdge(destinationVertex, ttf) {
        this->direction = direction;
    }

    bool isSameDirection(bool direction) {
        return (this->direction == direction);
    }

    bool getDirection() {
        return this->direction;
    }

private:
    bool direction; // true -> up; false -> down
};