
#include <cmath>
#include <fstream>
#include "../external_headers/json.hpp"
using json = nlohmann::json;
using std::ifstream;
using std::string;

class OSMGraph {
public:
    OSMGraph(const char* osmFilePath = "./OSM_graph_data/graph.json") {
        this->osmFilePath = osmFilePath;
    }

    /// Build a graph from OSM data and transform it to a strongly-connected simple graph. If 'car' is true, the edge weights are computed based on estimated car speeds. Otherwise, a walking speed of 5km/h is used.
    vector<vector<Edge>> build(bool car) {
        json j = importJson(this->osmFilePath);
        json features = j["features"];

        // Get number of vertices + coordinates (used to display graph)
        for (auto& feature : features) {
            if (feature["geometry"]["type"] == "Point") {
                this->verticesCoordinates.push_back(make_pair(feature["geometry"]["coordinates"][1], feature["geometry"]["coordinates"][0]));
            }
        }
        nbVertices = this->verticesCoordinates.size();
        
        // Init adjacency list
        vector<vector<Edge>> adjacencyList(nbVertices, vector<Edge>());

        // Get edges
        for (auto& feature : features) {
            if (feature["geometry"]["type"] == "LineString") {
                nbEdges++;
                int sourceVertexId = feature["src"];
                int targetVertexId = feature["tgt"];

                // Skip "loop edges"
                if (sourceVertexId == targetVertexId) {
                    nbLoopEdges++;
                    continue;
                }
                
                // Compute max speed
                float maxSpeed_kmh = 5.0f; // Assume 5km/h walking speed
                if (car) {
                    if (feature["properties"]["tags"]["maxspeed"].is_null()) {
                        maxSpeed_kmh = estimateMaxSpeed(feature["properties"]["tags"]["highway"]);
                    } else {
                        string maxspeed_str = feature["properties"]["tags"]["maxspeed"];
                        try {
                            maxSpeed_kmh = std::stof(maxspeed_str);
                        } 
                        catch (std::invalid_argument const &e) {
                            maxSpeed_kmh = estimateMaxSpeed(feature["properties"]["tags"]["highway"]);
                        }
                        catch (std::out_of_range const &e) {cout << "maxspeed cast error (out of range) : " << maxspeed_str << endl; exit(0);}
                    }
                }
                
                // Compute edge length
                float length_km = 0;
                bool previous = false;
                float previousLat = 0;
                float previousLong = 0;
                for (auto& coordinate : feature["geometry"]["coordinates"]) {
                    float currentLong = coordinate[0];
                    float currentLat = coordinate[1];
                    if (previous) {
                        length_km += distanceLatLong(currentLat, currentLong, previousLat, previousLong);
                    }
                    previousLong = currentLong;
                    previousLat = currentLat;
                    previous = true;
                }

                // Compute edge weight 
                float time_h = length_km / maxSpeed_kmh;
                float time_s = time_h * 3600;
                int weight = (int)time_s;

                // Add edge to graph
                int sourceVertex = sourceVertexId - 1; // -1 cause first vertex has id=1
                int targetVertex = targetVertexId - 1;
                addEdge(adjacencyList, sourceVertex, targetVertex, weight);

                // If two-way street : add opposite edge
                if (feature["properties"]["tags"]["oneway"].is_null() || feature["properties"]["tags"]["oneway"] == "no") {
                    this->nbTwoWay++;
                    addEdge(adjacencyList, targetVertex, sourceVertex, weight);
                }

            }
        }

        // Remove non-connected components
        vector<vector<Edge>> connectedGraph;
        while (this->connectedRatio < 0.5) {
            int s = Random::randomInt(adjacencyList.size());
            connectedGraph = transformToStronglyConnectedGraph(adjacencyList, s);
            this->connectedRatio = (float)connectedGraph.size()/(float)adjacencyList.size();
        }
        this->finalGraphVertices = connectedGraph.size();
        this->finalGraphEdges = GraphUtils::getNbEdges(connectedGraph);

        return connectedGraph;
    }
    
    vector<pair<float, float>> getVerticesCoordinates() {
        return this->verticesCoordinates;
    }

    void printImportStats() {
        cout << "------- Data Importation Stats -------" << endl;
        cout << "The original data had " << this->nbVertices << " vertices and " << this->nbEdges << " edges." << endl;
        cout << this->nbTwoWay <<  " (" << 100*(float)this->nbTwoWay/(float)this->nbEdges << "%)" << " two-way streets (added opposite edge)" << endl;
        cout << this->nbLoopEdges <<  " (" << 100*(float)this->nbLoopEdges/(float)this->nbEdges << "%)" << " loop edges (ignored)" << endl;
        cout << this->nbParallelEdges <<  " (" << 100*(float)this->nbParallelEdges/(float)this->nbEdges << "%)" << " parallel edges (minimum weight taken)" << endl;
        cout << "Max speed estimated for " << this->nbEdgesWithoutMaxSpeed << " edges " << "(" << 100*(float)this->nbEdgesWithoutMaxSpeed/(float)this->nbEdges << "%)" << endl;
        cout << "Transformed to strongly-connected graph : " << this->connectedRatio*100 << "% of vertices kept." << endl;
        cout << "The generated graph has " << this->finalGraphVertices << " vertices and " << this->finalGraphEdges << " edges." << endl;
        cout << "--------------------------------------" << endl;
    }

    /*
    Return the distance (in km) between two points expressed by their latitude and longitude. 
    It computes the euclidean distance between the two points, which is a good approx if the points are reasonably close (compared to the scale of the earth).
    */
    float distanceLatLong(float lat1, float long1, float lat2, float long2) {
        float phi1 = 90.0f-lat1;
        float phi2 = 90.0f-lat2;
        float phi1Radians = degreesToRadians(phi1);
        float phi2Radians = degreesToRadians(phi2);
        float theta1Radians = degreesToRadians(long1);
        float theta2Radians = degreesToRadians(long2);

        float R_EARTH = 6371; // in kilometers
        CartesianCoordinate p1 = sphericalToCartesian(R_EARTH, theta1Radians, phi1Radians);
        CartesianCoordinate p2 = sphericalToCartesian(R_EARTH, theta2Radians, phi2Radians);

        return distance(p1, p2);
    }

    /// Parse the OSM file containing all Villo stations in Brussels, and return the corresponding list of coordinates
    vector<pair<float, float>> getVilloStationsCoordinates() {
        vector<pair<float, float>> villoCoordinates;
        json j = importJson(PATH_OSM_GRAPHS "villo.json");
        json features = j["features"];
        for (auto& feature : features) {
            villoCoordinates.push_back(make_pair(feature["geometry"]["coordinates"][1], feature["geometry"]["coordinates"][0]));
        }
        return villoCoordinates;
    }

private:
    const char* osmFilePath;
    vector<pair<float, float>> verticesCoordinates; // for displaying graph; order : lat, long
    int nbEdgesWithoutMaxSpeed = 0;
    int nbLoopEdges = 0;
    int nbParallelEdges = 0;
    int nbEdges = 0;
    int nbVertices = 0;
    int nbTwoWay = 0;
    int finalGraphEdges = 0;
    int finalGraphVertices = 0;
    float connectedRatio = 0;

    /// Return the biggest strongly connected subgraph containing specified vertex 's'
    vector<vector<Edge>> transformToStronglyConnectedGraph(vector<vector<Edge>>& graph, int s) {
        vector<vector<Edge>> stronglyConnectedGraph;
        // Search all vertices reachable by 's'
        Dijkstra fromS(graph, s, -1); // t set to -1 to disable stopping criterion
        fromS.compute();
        vector<int> weightsFromS = fromS.getWeights();
        // Search all vertices that can reach 's'
        vector<vector<Edge>> reversedGraph = GraphUtils::reverseGraph(graph);
        Dijkstra toS(reversedGraph, s, -1); // t set to -1 to disable stopping criterion
        toS.compute();
        vector<int> weightsToS = toS.getWeights();
        // Intersect both vertices sets
        vector<bool> verticesToKeep(graph.size(), true);
        vector<int> offsets;
        int offset = 0;
        for (int vertex=0; vertex<graph.size(); vertex++) {
            if (weightsFromS[vertex] == -1 || weightsToS[vertex] == -1) {
                verticesToKeep[vertex] = false;
                offset++;
            }
            offsets.push_back(offset);
        }
        // Build induced subgraph
        vector<pair<float, float>> newCoordinates;
        for (int v = 0; v<graph.size(); v++) {
            if (verticesToKeep[v]) {
                vector<Edge> edges;
                for (auto& edge : graph[v]) {
                    if (verticesToKeep[edge.getDestinationVertex()]) {
                        edges.push_back(Edge(edge.getDestinationVertex()-offsets[edge.getDestinationVertex()], edge.getWeight()));
                    }
                }
                stronglyConnectedGraph.push_back(edges);
                newCoordinates.push_back(this->verticesCoordinates[v]);
            }
        }
        this->verticesCoordinates = newCoordinates;

        return stronglyConnectedGraph;
    }

    /// Add the specified edge. If a parallel edge already exists, replace it with the edge of minimum weight.
    void addEdge(vector<vector<Edge>>& adjacencyList, int u, int v, int newWeight) {
        bool alreadyEdge = false;
        for (auto& edge : adjacencyList[u]) {
            if (edge.getDestinationVertex() == v) {
                // There exists a u-v edge : it should be replaced by minimum weight
                alreadyEdge = true;
                edge.setMinWeight(newWeight);
            }
        }
        if (!alreadyEdge) {
            // Add edge
            Edge edge(v, (int)newWeight);
            adjacencyList[u].push_back(edge);
        }
        this->nbParallelEdges += alreadyEdge;
    }
    
    json importJson(const char* osmFilePath) {
        ifstream jsonFile(osmFilePath);
        json j;
        jsonFile >> j;
        jsonFile.close();
        return j;
    }

    /// Estimate the speed limit based on the route type
    float estimateMaxSpeed(string highwayType) {
        // No maxspeed available : estimate with highway type
        nbEdgesWithoutMaxSpeed++;
        if (highwayType == "primary" || highwayType == "primary_link" || highwayType == "secondary" || highwayType == "secondary_link" 
                            || highwayType == "tertiary" || highwayType == "tertiary_link" || highwayType == "residential") {
            return 50.0f;
        } else if (highwayType == "motorway" || highwayType == "motorway_link" || highwayType == "trunk" || highwayType == "trunk_link") {
            return 120.0f;
        } else {
            cout << "Error : Unknown highway type" << endl;
            exit(0);
        }
    }

    typedef struct {
        float x;
        float y;
        float z;
    } CartesianCoordinate;

    /// Compute the 3D euclidean distance between the 2 specified coordinates
    float distance(CartesianCoordinate p1, CartesianCoordinate p2) {
        return sqrt(pow(p1.x-p2.x, 2) + pow(p1.y-p2.y, 2) + pow(p1.z-p2.z, 2));
    }

    float degreesToRadians(float degrees) {
        return (degrees * M_PI) / 180.0;
    }

    CartesianCoordinate sphericalToCartesian(float r, float theta, float phi) {
        float x = r*sin(theta)*cos(phi);
        float y = r*sin(theta)*sin(phi);
        float z = r*cos(theta);
        return CartesianCoordinate{x, y, z};
    }
};



