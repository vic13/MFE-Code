
class Edge {
public:
    
    Edge(int destinationVertex, int weight) {
        this->destinationVertex = destinationVertex;
        this->weight = weight;
    }

    int getWeight() {
        return this->weight;
    }

    int getDestinationVertex() {
        return this->destinationVertex;
    }

    void setMinWeight(int weight) {
        if (weight < this->weight) {
            this->weight = weight;
        }
    }

protected:
    int destinationVertex;
    int weight;
};

class CHEdge : public Edge {
public:
    CHEdge(int sourceVertex, int destinationVertex, int weight): Edge(destinationVertex, weight) {
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
    CHQueryEdge(int destinationVertex, int weight, bool direction): Edge(destinationVertex, weight) {
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

    int evaluate(int t=0) {
        return this->ttf.evaluate(t);
    }

    int getDestinationVertex() {
        return this->destinationVertex;
    }

    void setMinWeight(TTF ttf) {
        this->ttf = TTF::minimum(ttf, this->ttf);
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

    int getMaxima() {
        return this->ttf.getMaxima();
    }

    int getMinima() {
        return this->ttf.getMinima();
    }

private:
    int sourceVertex;
};

class TCHQueryEdge : public TDEdge {
public:
    TCHQueryEdge(int destinationVertex, TTF ttf, bool direction, bool reversed = false): TDEdge(destinationVertex, ttf) {
        this->direction = direction;
        this->reversed = reversed;
    }

    bool isSameDirection(bool direction) {
        return (this->direction == direction);
    }

    bool getDirection() {
        return this->direction;
    }

    bool isReversed() {
        return this->reversed;
    }

    bool isReachable() {
        return this->reachable;
    }

    void markReachable() {
        this->reachable = true;
    }

private:
    bool direction; // true -> up; false -> down
    bool reachable = false;
    bool reversed;
};