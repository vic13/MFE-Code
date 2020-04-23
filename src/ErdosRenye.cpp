
// Victor Goossens
// 425554

#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;

class ErdosRenye {
    public:
        int n;
        int k;
        vector<vector<bool>> adjacencyMatrix;
        vector<int> degrees;

        ErdosRenye(int n, int k) {
            this->n = n;
            this->k = k;
            this->adjacencyMatrix = vector<vector<bool>>(n, vector<bool>(n, false));
            this->degrees = vector<int>(n, 0);
        }

        void build() {
            srand(time(NULL));

            // Add links until k links
            int numberOfLinks = 0;
            while (numberOfLinks < this->k) {
                bool succeeded = false;
                while (!succeeded) {
                    succeeded = tryToAddLink();
                }
                numberOfLinks++;
            }
        }

        bool tryToAddLink() {
            // Choose 2 nodes randomly
            int n1 = rand() % n;
            int n2 = rand() % n;
            // Check if already linked or if two identical nodes
            if (n1 == n2 || adjacencyMatrix[n1][n2]) {  
                return false;
            } else {
                addLink(n1, n2);
                return true;
            }
        }

        void addLink(int n1, int n2) {
            adjacencyMatrix[n1][n2] = true;
            adjacencyMatrix[n2][n1] = true;
            degrees[n1]++;
            degrees[n2]++;
        }

        vector<vector<unsigned short>> asNeighbours() {
            vector<vector<unsigned short>> nodes(n, vector<unsigned short>());
            for (int node = 0; node < n; node++) {
                vector<bool> neighbours = adjacencyMatrix[node];
                for (int neighbour = 0; neighbour < n; neighbour++) {
                    if (neighbours[neighbour]) {
                        nodes[node].push_back(neighbour);
                    }
                }
            }
            return nodes;
        }

};

int main() {
    // Parameters
    int n = 10000;
    int k = 2*n;
    
    // Build network
    ErdosRenye randomNetwork = ErdosRenye(n, k);
    randomNetwork.build();
    vector<vector<unsigned short>> transformedNetwork = randomNetwork.asNeighbours(); // Get the network as a list of n lists of neighbours
    cout << "Erdos-Renye network built" << endl;
}
