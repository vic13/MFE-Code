
#include "external_headers/matplotlibcpp.h"
namespace plt = matplotlibcpp;

class View {
public:
    static void display(vector<vector<Edge>> adjacencyList, vector<int> path, vector<pair<float, float>> nodesCoordinates) {
        // Display nodes
        vector<float> x;
        vector<float> y;
        for (auto& nodeCoordinates : nodesCoordinates) {
            x.push_back(nodeCoordinates.second);
            y.push_back(nodeCoordinates.first);
        }
        plt::scatter(x, y, 10);

        // Display edges
        for (int src=0; src<adjacencyList.size(); src++) {
            vector<Edge> edges = adjacencyList[src];
            for (auto& edge : edges) {
                int tgt = edge.getDestinationNode();
                vector<float> x{nodesCoordinates[src].second, nodesCoordinates[tgt].second};
                vector<float> y{nodesCoordinates[src].first, nodesCoordinates[tgt].first};
                plt::plot(x, y);
            }
        }

        plt::show();
        
    }
};