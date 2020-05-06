
#include "external_headers/matplotlibcpp.h"
namespace plt = matplotlibcpp;

class View {
public:
    static void display(vector<vector<Edge>> adjacencyList, vector<int> path, vector<pair<float, float>> nodesCoordinates) {
        plt::figure_size(700, 700);

        // Display nodes
        vector<float> xNodes;
        vector<float> yNodes;
        for (auto& nodeCoordinates : nodesCoordinates) {
            xNodes.push_back(nodeCoordinates.second);
            yNodes.push_back(nodeCoordinates.first);
        }
        plt::scatter(xNodes, yNodes, 10);

        // Display edges
        for (int src=0; src<adjacencyList.size(); src++) {
            vector<Edge> edges = adjacencyList[src];
            for (auto& edge : edges) {
                int tgt = edge.getDestinationNode();
                vector<float> xEdge{nodesCoordinates[src].second, nodesCoordinates[tgt].second};
                vector<float> yEdge{nodesCoordinates[src].first, nodesCoordinates[tgt].first};
                std::map<string,string> keywords; 
                keywords["alpha"] = "0.5";
                keywords["color"] = "black";
                plt::plot(xEdge, yEdge, keywords);
                // scatter marker close to edge src to show edge direction
                float directionMarkerX = 0.8*nodesCoordinates[src].second + 0.2*nodesCoordinates[tgt].second;
                float directionMarkerY = 0.8*nodesCoordinates[src].first + 0.2*nodesCoordinates[tgt].first;
                keywords["alpha"] = "1";
                keywords["color"] = "green";
                plt::scatter(vector<float>{directionMarkerX}, vector<float>{directionMarkerY}, 5, keywords);
            }
        }

        // Display shortest path
        vector<float> xPath;
        vector<float> yPath;
        for (auto& node : path) {
            xPath.push_back(nodesCoordinates[node].second);
            yPath.push_back(nodesCoordinates[node].first);
        }
        std::map<string,string> keywords; 
        keywords["linewidth"] = "3";
        keywords["alpha"] = "0.5";
        keywords["color"] = "red";
        plt::plot(xPath, yPath, keywords);

        // Display s and t
        std::map<string,string> keywordsST;
        keywordsST["alpha"] = "0.5";
        keywordsST["color"] = "green";
        plt::scatter(vector<float>{nodesCoordinates[path[0]].second}, vector<float>{nodesCoordinates[path[0]].first}, 100, keywordsST);
        keywordsST["color"] = "orange";
        plt::scatter(vector<float>{nodesCoordinates[path[path.size()-1]].second}, vector<float>{nodesCoordinates[path[path.size()-1]].first}, 100, keywordsST);

        
        plt::show();
        
    }
};