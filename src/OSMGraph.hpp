
#include <cmath>
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;
using std::ifstream;
using std::string;

class OSMGraph {
public:
    OSMGraph(const char* osmFilePath = "./OSM_graph_data/graph.json") {
        this->osmFilePath = osmFilePath;
    }

    vector<vector<Edge>> build() {
        json j = importJson();
        json features = j["features"];
        // Get number of nodes
        int nbNodes = 0;
        for (auto& feature : features) {
            if (feature["geometry"]["type"] == "Point") {
                nbNodes++;
            }
        }
        
        // Init adjacency list
        vector<vector<Edge>> adjacencyList(nbNodes, vector<Edge>());
        // Get edges
        int nbEdgesWithoutMaxSpeed = 0;
        int nbEdges = 0;
        for (auto& feature : features) {
            if (feature["geometry"]["type"] == "LineString") {
                int sourceNodeId = feature["src"];
                int targetNodeId = feature["tgt"];

                // Skip "loop edges"
                if (sourceNodeId == targetNodeId) {
                    continue;
                }
                
                // Compute max speed
                int maxSpeed_kmh = 0;
                if (feature["properties"]["tags"]["maxspeed"].is_null()) {
                    // No maxspeed available : estimate with highway type
                    nbEdgesWithoutMaxSpeed++;
                    maxSpeed_kmh = estimateMaxSpeed(feature["properties"]["tags"]["highway"]);
                } else {
                    string maxspeed_str = feature["properties"]["tags"]["maxspeed"];
                    try {
                        maxSpeed_kmh = std::stoi(maxspeed_str);
                    } 
                    catch (std::invalid_argument const &e) {cout << "maxspeed cast error" << endl; exit(0);}
                    catch (std::out_of_range const &e) {cout << "maxspeed cast error" << endl; exit(0);}
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
                float weight = time_s;

                // Add edge to graph
                int sourceNode = sourceNodeId - 1; // -1 cause first node has id=1
                int targetNode = targetNodeId - 1;
                Edge edge(targetNode, weight);
                adjacencyList[sourceNode].push_back(edge);

            }
        }

        return adjacencyList;
    }

private:
    const char* osmFilePath;
    
    json importJson() {
        ifstream jsonFile(osmFilePath);
        json j;
        jsonFile >> j;
        jsonFile.close();
        return j;
    }

    int estimateMaxSpeed(string highwayType) {
        if (highwayType == "primary" || highwayType == "primary_link" || highwayType == "secondary" || highwayType == "secondary_link" 
                            || highwayType == "tertiary" || highwayType == "tertiary_link" || highwayType == "residential") {
            return 50;
        } else if (highwayType == "motorway" || highwayType == "motorway_link" || highwayType == "trunk" || highwayType == "trunk_link") {
            return 120;
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

    /*
    Returns the distance (in km) between two points expressed by their latitude and longitude. 
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

    float distance(CartesianCoordinate p1, CartesianCoordinate p2) {
        return sqrt(pow(p1.x-p2.x, 2) + pow(p1.y-p2.y, 2) + pow(p1.z-p2.z, 2));
    }

    float degreesToRadians(float degrees) {
        return (degrees * M_PI) / 180;
    }

    CartesianCoordinate sphericalToCartesian(float r, float theta, float phi) {
        float x = r*sin(theta)*cos(phi);
        float y = r*sin(theta)*sin(phi);
        float z = r*cos(theta);
        return CartesianCoordinate{x, y, z};
    }
};