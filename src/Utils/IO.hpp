
#include <fstream>
#include <sstream>
using std::stringstream;

class IO {
public:
    static void writeGraphToFile(const char* filePath, vector<vector<CHQueryEdge>> graph) {
        std::ofstream ofs(filePath);
        for (auto& vertexEdges : graph) {
            for (auto& edge : vertexEdges) {
                ofs << edge.getDestinationVertex() << "," << edge.getWeight() << "," << edge.getDirection() << ",";
            }
            ofs << ";,";
        }
        ofs.close();
    }

    static void writeToFile(string filePath, string s) {
        std::ofstream ofs(filePath);
        ofs << s;
        ofs.close();
    }

    static vector<vector<CHQueryEdge>> readGraphFromFile(const char* filePath) {
        std::ifstream ifs(filePath);
        vector<vector<CHQueryEdge>> graph;
        vector<CHQueryEdge> edges;
        string s;
        int dest = 0;
        int w = 0;
        bool direction = 0;
        int state = 0;
        while (getline(ifs, s, ',')) {
            stringstream ss(stringstream::in | stringstream::out);
            ss << s;
            if (s==";") {
                graph.push_back(edges);
                edges.clear();
            } else {
                if (state == 0) {
                    ss >> dest;
                    state = 1;
                } else if (state == 1) {
                    ss >> w;
                    state = 2;
                } else if (state == 2) {
                    ss >> direction;
                    state = 0;
                    edges.push_back(CHQueryEdge(dest, w, direction));
                }
            }
        }
        ifs.close();
        return graph;
    }
};