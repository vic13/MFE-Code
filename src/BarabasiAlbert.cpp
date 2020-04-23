// Victor Goossens
// 425554


#include <vector>
#include <iostream>
#include <chrono>

using std::cout;
using std::endl;
using std::vector;

#include "Edge.hpp"
#include "Dijkstra.hpp"

class BarabasiAlbert {
    public:
        int n;
        vector<vector<float>> adjacencyMatrix;
        vector<int> degrees;
        int degreeSum;

        BarabasiAlbert(int n) {
            this->n = n;
            this->adjacencyMatrix = vector<vector<float>>(n, vector<float>(n, -1.0f)); // -1 corresponds to no connection (or infinite weight)
            this->degrees = vector<int>(n, 0);
        }

        void build() {
            srand(time(NULL));

            // Start with 4 connected nodes
            for (int i=0; i<4; i++) {
                for (int j=0; j<4; j++) {
                    if (i!=j) {
                        adjacencyMatrix[i][j] = 10;
                    }
                }
                degrees[i] = 6;
            }
            degreeSum = 4*6;

            // Add nodes until n nodes
            for (int n1=4; n1<n; n1++) {
                // Attach 4 links (proportionally to node degree)
                for (int link=0; link<4; link++) {
                    bool succeeded = false;
                    while (!succeeded) {
                        succeeded = tryToAddLink(n1);
                    }
                }
            }
        }

        bool tryToAddLink(int n1) {
            int r = rand() % degreeSum;
            int cumulDegree = 0;
            for (int n2 = 0; n2<=n1; n2++) {
                cumulDegree += degrees[n2];
                if (r<cumulDegree) {
                    if (adjacencyMatrix[n1][n2] >= 0 || n1==n2) { // Check if already linked or if two identical nodes
                        return false;
                    } else {
                        addLink(n1, n2);
                        return true;
                    }
                }
            }
            cout << "Error : end of non-void function" << endl;
            exit(0);
        }

        void addLink(int n1, int n2) {
            float someWeight = 10.0f;
            adjacencyMatrix[n1][n2] = someWeight;
            //adjacencyMatrix[n2][n1] = true;
            degrees[n1]++;
            degrees[n2]++;
            degreeSum+=2;
        }

        vector<vector<Edge>> asNeighbours() {
            vector<vector<Edge>> nodes(n, vector<Edge>());
            for (int node = 0; node < n; node++) {
                vector<float> neighbours = adjacencyMatrix[node];
                for (int neighbour = 0; neighbour < n; neighbour++) {
                    float weight = neighbours[neighbour];
                    if (weight >= 0) {  // negative value corresponds to no connection
                        Edge edge = Edge(neighbour, weight);
                        nodes[node].push_back(edge);
                    }
                }
            }
            return nodes;
        }

};


int main() {
    cout << "hello world" << endl;
    auto t1 = std::chrono::high_resolution_clock::now();

    // Parameters
    int n = 1000;

    // Build network
    BarabasiAlbert scaleFreeNetwork = BarabasiAlbert(n);
    scaleFreeNetwork.build();
    auto t2 = std::chrono::high_resolution_clock::now();
    auto buildDuration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout << "Build duration : " << buildDuration/1000000.0 << endl;

    vector<vector<Edge>> graph = scaleFreeNetwork.asNeighbours(); // Get the network as a list of n lists of neighbours
    auto t3 = std::chrono::high_resolution_clock::now();
    auto convertDuration = std::chrono::duration_cast<std::chrono::microseconds>( t3 - t2 ).count();
    std::cout << "Convert duration : " << convertDuration/1000000.0 << endl;

    /*for (int a=0; a<100; a++) {
        vector<Edge> neighbours = graph[a];
        cout << "nb neighbours : " << neighbours.size() << endl;
        for (int b=0; b<neighbours.size(); b++) {
            cout << "weight : " << neighbours[b].getWeight() << endl;
            cout << "dest : " << neighbours[b].getDestinationNode() << endl;
        }
    }*/

    Dijkstra dijkstra(graph, 234, 400);
    dijkstra.compute();
    
}


