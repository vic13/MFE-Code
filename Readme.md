
# Usage
A Makefile is provided. Simply run :
``` 
$ make
```
in a terminal to compile and run the code. The code execution depends on the value of parameters that you can specify in the file `src/parameters.hpp`. The principal parameters are :
- `PARAMS_GRAPH_NAME` : the name of the OSM graph that will be used during the experiments. In the thesis, 5 graphs of increasing size were used. However, the last two were too large to be uploaded on GitHub. Therefore, only 3 are provided. The graph number 3 represents Brussels and was used for experiments 2, 3 and 4 in the thesis. The graphs 4 and 5 were only used in single-modal experiments. The OSM graphs are stored in the directory `src/graph_data_OSM/`. They were generated using the tool GraphFromOSM (https://github.com/MatveiT/GraphFromOSM). To specify which of the 3 provided graphs will be used, set the macro `PARAMS_GRAPH_NAME` to one of the following : `GRAPH_NAME_1`, `GRAPH_NAME_2`, `GRAPH_NAME_3`.
- `PARAMS_EXP_NB` : the number of the experiment to be run. It can have the following values :
  - `1` : Benchmark Contraction Hierarchies preprocessing and querying (against Dijkstra) on a single-modal car network build from the OSM graph specified by `PARAMS_GRAPH_NAME`. The results are stored in a file named `<PARAMS_GRAPH_NAME>.txt`.
  - `2` : Benchmark Contraction Hierarchies preprocessing and querying (against Dijkstra) on a multi-modal network composed of a walk layer (build from the OSM graph specified by `PARAMS_GRAPH_NAME`) and additional "lines" modeling public transportation. The benchmarking is done for various number of added lines `<nb>` and line speeds `<s>`. The results are stored in files named `<graph_name>-s<s>.txt`. Each file aggregates the results over all values of `<nb>` for a given speed `<s>`.
  - `3` : Benchmark Time-dependent Contraction Hierarchies preprocessing and querying (against Time-dependent Dijkstra) on a multi-modal network similar to the one of exp. 2. The benchmarking is done for various number of added lines `<nb>` and 2 types `<type>` of weight for the board edges : null weight and waiting times. The results are stored in files named `<graph_name>-type<type>.txt`. Each file aggregates the results over all values of `<nb>` for a given type `<type>`.
  - `4` : Benchmark Contraction Hierarchies preprocessing and querying (against Dijkstra) on a station-based multi-modal network, composed of a walk layer and a bike layer, both build from the OSM graph specified by `PARAMS_GRAPH_NAME`, and board/alight edges at bike stations. The data for bike stations is located in the file `src/graph_data_OSM/villo.json`, and contains all "Villo" stations in the region of Brussels. This experiment should therefore be run using the OSM graph 3 representing Brussels. The results are stored in a file named `<PARAMS_GRAPH_NAME>.txt`.

  The results files are stored in the directory `src/benchmarks/<i>/`, where `<i>` is the number of the experiment.
- `PARAMS_SEED` : the random seed used during the experiments (e.g.: to generate random pairs of vertices for the queries, and to generate random waiting times TTF for exp. 3). The value `-1` allows to set the random seed based on the current machine time.
- `PARAMS_QUERY_STALL` : specify if Stall-on-Demand is used during Static Contraction Hierarchies queries.
- `PARAMS_NB_RUNS_QUERY_BENCHMARK` : the number of queries performed for both Dijkstra and CH, during the query benchmarking.

# Source files and directories

- `Benchmark.hpp` : Implements the preprocessing and querying benchmark functions, as well as the experiments functions, building the required graphs and saving the benchmarking results to files.
- `benchmarks/` : Stores the results of the experiments.
- `CH/` : Contains classes implementing Static Contraction Hierarchies (CH) and Time-dependent Contraction Hierarchies (TCH) preprocessing and query functions. `CHTemplate.hpp` is a template for the preprocessing, specialized for CH in `CH.hpp` and for TCH in `TCH.hpp`. The query algorithms for CH and TCH are implemented in `CHQuery.hpp` and `TCHQuery.hpp` respectively.
- `Dijkstra.hpp` : Implements Dijkstra's algorithm and its time-dependent variant.
- `Edge.hpp` : Defines the multiple types of edges required during Dijkstra querying (`Edge`, `TDEdge`), CH preprocessing (`CHEdge`, `TCHEdge`) and CH querying (`CHQueryEdge`, `TCHQueryEdge`).
- `external_headers/` : Contains two external headers, one helping parsing .json files, the other proposing a C++ interface for Python's "matplotlib" library.
- `graph_data_OSM/` : Contains OSM graph data in the geoJSON format.
- `Graph_Generation/` : Contains classes able to generate graphs as adjacency lists. Because we focused on road networks, the only one used in the experiments is `OSMGraph.hpp`, building graphs from the data in `graph_data_OSM/`. The files `BarabasiAlbertGraph.hpp` and `ErdosRenyi.hpp` allows to generate scale-free networks and random networks respectively. They are proposed to allow further experiments of Contraction Hierarchies with these different graph topologies.
- `main.cpp` : Launches the correct experiment based on the value specified in `parameters.hpp`.
- `parameters.hpp` : Cf. "Usage".
- `TTF.hpp` : Implements Travel-Time Functions and their 3 basic operations : "minimum", "evaluate" and "chaining".
- `Utils/` : Defines miscellaneous utility functions.
- `View.hpp` : Provides functions to display small networks, paths, and TTF (used primarily for debugging). It relies on the "matplotlib" interface in `external_headers/`. To use it, it is required to activate `PARAMS_VIEW` in `parameters.hpp`, to have Python installed and `<Python.h>` part of your include paths. 