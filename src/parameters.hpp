
#define PATH_OSM_GRAPHS                             "./graph_data_OSM/"
#define OSM_GRAPHS_EXTENSION                        ".json"
#define PATH_CH_GRAPHS                              "./graph_data_CH/"
#define CH_GRAPHS_EXTENSION                         ".txt"
#define PATH_BENCHMARKS                             "./benchmarks/"
#define BENCHMARKS_EXTENSION                        ".txt"
#define GRAPH_NAME_1                                "graph_1_ULB"           // Area around ULB (Ixelles)
#define GRAPH_NAME_2                                "graph_2_bxl_ctr"       // Center of Brussels
#define GRAPH_NAME_3                                "graph_3_bxl"           // Brussels
#define GRAPH_NAME_4                                "graph_4_be_ctr"        // Center of Belgium (Not on github because too large)
#define GRAPH_NAME_5                                "graph_5_be"            // Belgium (Not on github because too large)

#define PARAMS_GRAPH_NAME                           GRAPH_NAME_3            // Graph used during the experiments
#define PARAMS_EXP_NB                               2                       // Between 1 and 4. (Cf. Readme.md for details)
#define PARAMS_SEED                                 123                     // -1 for changing seed (based on system time)
#define PARAMS_PREPROCESSING_BENCHMARK              true                    // Set to 'true' to run preprocessing benchmark and save results to file
#define PARAMS_QUERY_BENCHMARK                      true                    // Set to 'true' to run query benchmark and save results to file
#define PARAMS_QUERY_STALL                          true                    // Set to 'true' to apply Stall-on-Demand during CH querying
#define PARAMS_NB_RUNS_QUERY_BENCHMARK              1000                    // Number of queries run during query benchmarking
#define PARAMS_VIEW                                 false                   // Set to true to use visualisation functions in 'View.hpp'