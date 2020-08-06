
#define PATH_OSM_GRAPHS                             "./graph_data_OSM/"
#define OSM_GRAPHS_EXTENSION                        ".json"
#define PATH_CH_GRAPHS                              "./graph_data_CH/"
#define CH_GRAPHS_EXTENSION                         ".txt"
#define GRAPH_NAME_1                                "graph_1_test"
#define GRAPH_NAME_2                                "graph_2_bxl_ctr"
#define GRAPH_NAME_3                                "graph_3_bxl"
#define GRAPH_NAME_4                                "graph_4_be_ctr"        // Not on github because too large
#define GRAPH_NAME_5                                "graph_5_be"            // Not on github because too large

#define PARAMS_GRAPH_NAME                           GRAPH_NAME_2
#define PARAMS_READ_CH_FROM_FILE                    false
#define PARAMS_WRITE_CH_TO_FILE                     true                   // Only used if not 'PARAMS_READ_CH_FROM_FILE'
#define PARAMS_SEED                                 123                      // -1 for random seed
#define PARAMS_QUERY_BENCHMARK                      true
#define PARAMS_NB_RUNS_QUERY_BENCHMARK              100
#define PARAMS_VIEW                                 false