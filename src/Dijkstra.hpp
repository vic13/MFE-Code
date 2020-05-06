
#include <set>
using std::set;


class Dijkstra {
public:
    Dijkstra(vector<vector<Edge>> graph, int s, int t) {
        this->graph = graph;
        this->t = t;
        this->s = s;
        this->nodeSet = initNodeSet(s);
        this->nodeWeights = initNodeWeights(s);
        this->nodeParents = initNodeParents();
    }

    bool compute() {
        while (!nodeSet.empty()) {
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
                for (auto& e : edges) {
                    float node2CurrentWeight = nodeWeights[e.getDestinationNode()];
                    float node2NewWeight = nodeCurrentWeight + e.getWeight();
                    if ((node2CurrentWeight == -1.0f) || (node2NewWeight < node2CurrentWeight)) {                               // if smaller weight was found
                        if (node2CurrentWeight != -1.0f) {                                                              
                            // if current weight not infinite : node already in queue : DELETE before inserting the updated node
                            pair<float, int> currentNode2 = make_pair(node2CurrentWeight, e.getDestinationNode());
                            nodeSet.erase(nodeSet.find(currentNode2));
                        }
                        // INSERT in queue
                        pair<float, int> newNode2 = make_pair(node2NewWeight, e.getDestinationNode());
                        nodeSet.insert(newNode2);
                        // UPDATE weight + parent
                        nodeWeights[e.getDestinationNode()] = node2NewWeight;
                        nodeParents[e.getDestinationNode()] = node;
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

    vector<int> getPath() {
        // retrace path from end to start
        vector<int> path;
        path.push_back(this->t);
        int parentNode = this->nodeParents[this->t];
        path.push_back(parentNode);
        while (parentNode != this->s) {
            parentNode = this->nodeParents[parentNode];
            path.push_back(parentNode);
        }
        // reverse path
        std::reverse(path.begin(),path.end());

        return path;
    }

    void printNodeWeights() {
        print_vector(this->nodeWeights);
    }
  

private:
    int t;
    int s;
    vector<vector<Edge>> graph;
    set<pair<float, int>> nodeSet;
    vector<float> nodeWeights;
    vector<int> nodeParents;

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

    vector<int> initNodeParents() {
        return vector<int>(graph.size(), -1); // -1 corresponds to no parent
    }
    
};