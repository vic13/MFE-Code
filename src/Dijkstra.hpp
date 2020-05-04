
#include <set>

using std::set;
using std::pair;
using std::make_pair;

void print_vector(std::vector<int> const &input) {
	std::copy(input.begin(), input.end(), std::ostream_iterator<int>(cout, " "));
}
void print_vector(std::vector<float> const &input) {
	std::copy(input.begin(), input.end(), std::ostream_iterator<float>(cout, " "));
}

class Dijkstra {
public:
    Dijkstra(vector<vector<Edge>> graph, int s, int t) {
        this->graph = graph;
        this->t = t;
        this->nodeSet = initNodeSet(s);
        this->nodeWeights = initNodeWeights(s);
    }

    bool compute() {

        while (!nodeSet.empty()) {
            cout << nodeSet.size() << endl;
            // pop first node in the set
            pair<float, int> visitedNode = *(nodeSet.begin());
            nodeSet.erase(nodeSet.begin());
            float nodeCurrentWeight = visitedNode.first;
            int node = visitedNode.second;

            if (node == t) {
                // finished
                return true;
            } else {
                vector<Edge> edges = graph[node];
                for (int index=0; index<edges.size(); index++) {
                    Edge e = edges[index];
                    float node2CurrentWeight = nodeWeights[e.getDestinationNode()];
                    int node2NewWeight = nodeCurrentWeight + e.getWeight();
                    if ((node2CurrentWeight == -1.0f) || (node2NewWeight < node2CurrentWeight)) {                               // if smaller weight was found
                        if (node2CurrentWeight != -1.0f) {                                                              
                            // if current weight not infinite : node already in queue : DELETE before inserting the updated node
                            pair<float, int> currentNode2 = make_pair(node2CurrentWeight, e.getDestinationNode());
                            nodeSet.erase(nodeSet.find(currentNode2));
                        }
                        // INSERT in queue
                        pair<float, int> newNode2 = make_pair(node2NewWeight, e.getDestinationNode());
                        nodeSet.insert(newNode2);
                        // UPDATE weight
                        nodeWeights[e.getDestinationNode()] = node2NewWeight;
                    }
                }
            }
        }
        // No solution
        return false;
    }

    float getPathWeight() {
        return this->nodeWeights[this->t];
    }

    void printNodeWeights() {
        print_vector(this->nodeWeights);
    }
  

private:
    int t;
    vector<vector<Edge>> graph;
    set<pair<float, int>> nodeSet;
    vector<float> nodeWeights;

    vector<int> pathSequence;

    set<pair<float, int>> initNodeSet(int s) {
        set<pair<float, int>> nodeSet;
        nodeSet.insert(make_pair(0.0f, s)); 
        return nodeSet;
    }

    vector<float> initNodeWeights(int s) {
        vector<float> nodeWeights(graph.size(), -1); // -1 corresponds to infinite weight
        nodeWeights[s] = 0;
        return nodeWeights;
    }
    
};