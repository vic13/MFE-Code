
#include <queue>



class DijkstraNode {
public:
    DijkstraNode(int node, float currentShortest) { // currentShortest and previousNode set to -1 while no path found
        this->node = node;
        this->currentShortest = currentShortest; 
        this->visited = false;
        this->previousNode = -1;
    }

    int getNode() const {
        return this->node;
    }

    int getPreviousNode() const {
        return this->previousNode;
    }

    float getCurrentShortest() const {
        return this->currentShortest;
    }

private:
    int node;
    int previousNode;
    bool visited;
    float currentShortest;
};

class myComparator 
{ 
public: 
    int operator() (const DijkstraNode& n1, const DijkstraNode& n2) {
        if (n1.getCurrentShortest() < 0) {
            return true;
        } else if (n2.getCurrentShortest() < 0) {
            return false;
        } else {
            return (n1.getCurrentShortest() > n2.getCurrentShortest());
        }
    } 
};

void print_queue(std::priority_queue<DijkstraNode,  vector<DijkstraNode>, myComparator> q) {
    while(!q.empty()) {
        std::cout << "Node : " << q.top().getNode() << " Current shortest : " << q.top().getCurrentShortest() << " Previous node : " << q.top().getPreviousNode() << endl;
        q.pop();
    }
    std::cout << '\n';
}

class Dijkstra {
public:
    Dijkstra(vector<vector<Edge>> graph, int s, int t) {
        this->graph = graph;
        this->s = s;
        this->t = t;
        this->priorityQueue = initPriorityQueue(graph, s);
        print_queue(this->priorityQueue);
    }

    void compute() {
        
    }

private:
    vector<vector<Edge>> graph;
    int s;
    int t;
    std::priority_queue<DijkstraNode,  vector<DijkstraNode>, myComparator> priorityQueue;

    std::priority_queue<DijkstraNode,  vector<DijkstraNode>, myComparator> initPriorityQueue(vector<vector<Edge>> graph, int s) {
        std::priority_queue<DijkstraNode,  vector<DijkstraNode>, myComparator> priorityQueue;
        for (int node=0; node<graph.size(); node++) {
            float currentShortest = (node == s) ? 0 : -1;
            //float currentShortest = rand() % 1000 - 500;
            if (currentShortest == 0) {
                cout << s << endl;
            }
            const DijkstraNode dn(node, currentShortest);
            priorityQueue.push(dn);
        }
        return priorityQueue;
    }
};