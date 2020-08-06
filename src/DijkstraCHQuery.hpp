
#include <set>
using std::set;
#include <queue>
using std::queue;

class DijkstraCHQuery {
public:
    DijkstraCHQuery(vector<vector<CHQueryEdge>>& graph, int s, int t) {
        this->graph = graph;
        this->t = t;
        this->s = s;
        this->vertexSet_up = initVertexSet(s);
        this->vertexSet_down = initVertexSet(t);
        this->vertexSet = &vertexSet_up;
        this->vertexWeights_up = initVertexWeights(s);
        this->vertexWeights_down = initVertexWeights(t);
        this->vertexWeights = &vertexWeights_up;
        this->vertexParents_up = initVertexParents();
        this->vertexParents_down = initVertexParents();
        this->vertexParents = &vertexParents_up;
        this->stallingDistances_up = vector<float>(graph.size(), -1);
        this->stallingDistances_down = vector<float>(graph.size(), -1);
        this->stallingDistances = &stallingDistances_up;
    }

    bool compute() {
        while (!vertexSet_up.empty() || !vertexSet_down.empty()) {
            searchSpace++;
            // Set current direction and associated vertex set, weights, and parents 
            setDirection();

            // pop first vertex in the set
            pair<float, int> visitedVertex = *(vertexSet->begin());
            vertexSet->erase(vertexSet->begin());
            float visitedVertexWeight = visitedVertex.first;
            int visitedVertexNb = visitedVertex.second;
            updateD(visitedVertexNb);

            // Check for stopping criteria
            if (checkStoppingCriteria(visitedVertexWeight)) {
                return true;
            }

            // Stall-on-demand
            stallOnDemand(visitedVertexNb, visitedVertexWeight);
            if ((*stallingDistances)[visitedVertexNb] != -1) continue;

            for (auto& e : graph[visitedVertexNb]) {
                if (e.isSameDirection(direction)) {
                    float neighbourCurrentWeight = (*vertexWeights)[e.getDestinationVertex()];
                    float neighbourNewWeight = visitedVertexWeight + e.getWeight();
                    if ((neighbourCurrentWeight == -1.0f) || (neighbourNewWeight < neighbourCurrentWeight)) {    // if smaller weight was found
                        (*stallingDistances)[e.getDestinationVertex()] = -1; // Unstall
                        if (neighbourCurrentWeight != -1.0f) {                                                              
                            // if current weight not infinite : vertex already in queue : DELETE before inserting the updated vertex
                            pair<float, int> currentNeighbour = make_pair(neighbourCurrentWeight, e.getDestinationVertex());
                            vertexSet->erase(vertexSet->find(currentNeighbour));
                        }
                        // INSERT in queue
                        pair<float, int> newNeighbour = make_pair(neighbourNewWeight, e.getDestinationVertex());
                        vertexSet->insert(newNeighbour);
                        // UPDATE weight + parent
                        (*vertexWeights)[e.getDestinationVertex()] = neighbourNewWeight;
                        (*vertexParents)[e.getDestinationVertex()] = visitedVertexNb;
                    }
                }
            }
        }
        // Stopping criterion not met
        return (d != -1);
    }

    void stallOnDemand(int u, float du) {
        if ((*stallingDistances)[u] == -1) {
            for (auto& e : graph[u]) {
                if (!e.isSameDirection(direction)) {
                    int v = e.getDestinationVertex();
                    float dv = (*vertexWeights)[v];
                    if ((dv != -1) && (dv + e.getWeight() < du)) {
                        (*stallingDistances)[u] = dv + e.getWeight();
                        // stallPropagate(u);
                        break;
                    }
                }
            }
        }
    }

    void stallPropagate(int u) {
        float delta = 0.001;
        queue<int> q({u});
        while (!q.empty()) {
            int x = q.front();
            q.pop();
            for (auto& edge : graph[x]) {
                if (edge.isSameDirection(direction)) {
                    int v = edge.getDestinationVertex();
                    float newDistance = (*stallingDistances)[x] + edge.getWeight();
                    if (((*vertexWeights)[v] == -1) || ((*stallingDistances)[v] != -1) || ((*vertexWeights)[v] <= newDistance + delta)) { // if unreached, already stalled, or no better distance
                        continue;
                    }
                    (*stallingDistances)[v] = newDistance;
                    q.push(v);
                }
            }
        }
    }

    float getPathWeight() {
        return this->d;
    }

    int getSearchSpace() {
        return this->searchSpace;
    }

    vector<int> getPath() {
        // retrace path from end to start
        vector<int> path;
        path.push_back(this->t);
        if (this->s == this->t) {
            return path;
        }
        int parentVertex = (*this->vertexParents)[this->t];
        path.push_back(parentVertex);
        while (parentVertex != this->s) {
            parentVertex = (*this->vertexParents)[parentVertex];
            path.push_back(parentVertex);
        }
        // reverse path
        std::reverse(path.begin(),path.end());

        return path;
    }

    void printVertexWeights() {
        print_vector(*(this->vertexWeights));
    }
  

private:
    int t;
    int s;
    vector<vector<CHQueryEdge>> graph;
    bool direction = true; // Start with the upward search
    set<pair<float, int>>* vertexSet;
    set<pair<float, int>> vertexSet_up;
    set<pair<float, int>> vertexSet_down;
    vector<float>* vertexWeights;
    vector<float> vertexWeights_up;
    vector<float> vertexWeights_down;
    vector<int>* vertexParents;
    vector<int> vertexParents_up;
    vector<int> vertexParents_down;
    vector<float>* stallingDistances;
    vector<float> stallingDistances_up;
    vector<float> stallingDistances_down;
    float weight_upward_search = 0;
    float weight_downward_search = 0;
    float d = -1; // best path weight so far (-1 : infinity)
    int searchSpace = 0;

    set<pair<float, int>> initVertexSet(int s) {
        set<pair<float, int>> vertexSet;
        vertexSet.insert(make_pair(0.0f, s)); 
        return vertexSet;
    }

    vector<float> initVertexWeights(int s) {
        vector<float> vertexWeights(graph.size(), -1); // -1 corresponds to infinite weight
        vertexWeights[s] = 0;
        return vertexWeights;
    }

    vector<int> initVertexParents() {
        return vector<int>(graph.size(), -1); // -1 corresponds to no parent
    }

    bool checkStoppingCriteria(float visitedWeight) {
        // Check min queue weight
        if (this->direction) {
            weight_upward_search = visitedWeight;
        } else {
            weight_downward_search = visitedWeight;
        }

        float minQueueWeight = fmin(weight_downward_search, weight_upward_search);
        return (this->d != -1 && this->d <= minQueueWeight); // No possible better path can be found
    }

    void setDirection() {
        if (this->direction && !this->vertexSet_down.empty()) {
            // Switch down
            this->direction = false;
            this->vertexSet = &this->vertexSet_down;
            this->vertexWeights = &this->vertexWeights_down;
            this->vertexParents = &this->vertexParents_down;
            this->stallingDistances = &this->stallingDistances_down;
        } else if (!this->direction && !this->vertexSet_up.empty()) {
            // Switch up
            this->direction = true;
            this->vertexSet = &this->vertexSet_up;
            this->vertexWeights = &this->vertexWeights_up;
            this->vertexParents = &this->vertexParents_up;
            this->stallingDistances = &this->stallingDistances_up;
        }
    }

    void updateD(int visitedVertexNb) {
        float weight_up = this->vertexWeights_up[visitedVertexNb];
        float weight_down = this->vertexWeights_down[visitedVertexNb];
        if (weight_up != -1 && weight_down != -1) { // vertex reached in both direction
            if (weight_up + weight_down < this->d || this->d == -1) {
                this->d = weight_up + weight_down;
            }
        }
    }
    
};