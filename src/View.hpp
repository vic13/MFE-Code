
#include "external_headers/matplotlibcpp.h"
namespace plt = matplotlibcpp;

class View {
public:
    template <typename TEdge>
    static void displayNetwork(vector<vector<TEdge>> adjacencyList, vector<pair<float, float>> verticesCoordinates) {
        plt::figure_size(700, 700);

        // Display vertices
        vector<float> xVertices;
        vector<float> yVertices;
        for (auto& vertexCoordinates : verticesCoordinates) {
            xVertices.push_back(vertexCoordinates.second);
            yVertices.push_back(vertexCoordinates.first);
        }
        plt::scatter(xVertices, yVertices, 10);

        // Display edges
        for (int src=0; src<adjacencyList.size(); src++) {
            vector<TEdge> edges = adjacencyList[src];
            for (auto& edge : edges) {
                int tgt = edge.getDestinationVertex();
                vector<float> xEdge{verticesCoordinates[src].second, verticesCoordinates[tgt].second};
                vector<float> yEdge{verticesCoordinates[src].first, verticesCoordinates[tgt].first};
                std::map<string,string> keywords; 
                keywords["alpha"] = "0.5";
                keywords["color"] = "black";
                plt::plot(xEdge, yEdge, keywords);
                // scatter marker close to edge src to show edge direction
                float directionMarkerX = 0.8*verticesCoordinates[src].second + 0.2*verticesCoordinates[tgt].second;
                float directionMarkerY = 0.8*verticesCoordinates[src].first + 0.2*verticesCoordinates[tgt].first;
                keywords["alpha"] = "1";
                keywords["color"] = "green";
                plt::scatter(vector<float>{directionMarkerX}, vector<float>{directionMarkerY}, 5, keywords);
            }
        }

        plt::show();
    }

    template <typename TEdge>
    static void displayPath(vector<vector<TEdge>> adjacencyList, vector<int> path, vector<pair<float, float>> verticesCoordinates, int s, int t) {
         // Display shortest path
        vector<float> xPath;
        vector<float> yPath;
        for (auto& vertex : path) {
            xPath.push_back(verticesCoordinates[vertex].second);
            yPath.push_back(verticesCoordinates[vertex].first);
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
        plt::scatter(vector<float>{verticesCoordinates[s].second}, vector<float>{verticesCoordinates[s].first}, 100, keywordsST);
        keywordsST["color"] = "orange";
        plt::scatter(vector<float>{verticesCoordinates[t].second}, vector<float>{verticesCoordinates[t].first}, 100, keywordsST);

        displayNetwork(adjacencyList, verticesCoordinates);
    }

    static void displayTTF(vector<TTF> ttfs) {
        
        plt::figure_size(700, 400);
        for (int i = 0; i<ttfs.size(); i++) {
            TTF f = ttfs[i];
            vector<float> x;
            vector<float> y;
            for (auto& point : f.getPoints()) {
                x.push_back(point.first);
                y.push_back(point.second);
            }
            std::map<string,string> keywords;
            keywords["marker"] = ".";
            if (i==ttfs.size()-1) {
                keywords["linewidth"] = "5";
                keywords["alpha"] = "0.3";
            }
            plt::plot(x, y, keywords);
        }
        
        // plt::ylim(-1,20);
        plt::xlim(-50,(int)TTF::period+50);
        plt::show();
    }

};