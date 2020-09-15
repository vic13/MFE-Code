
#define PATH_OSM_GRAPHS                             "./graph_data_OSM/"
#define OSM_GRAPHS_EXTENSION                        ".json"
#define PATH_CH_GRAPHS                              "./graph_data_CH/"
#define CH_GRAPHS_EXTENSION                         ".txt"
#define PATH_BENCHMARKS                             "./benchmarks/"
#define BENCHMARKS_EXTENSION                        ".txt"
#define GRAPH_NAME_1                                "graph_1_test"
#define GRAPH_NAME_2                                "graph_2_bxl_ctr"
#define GRAPH_NAME_3                                "graph_3_bxl"
#define GRAPH_NAME_4                                "graph_4_be_ctr"        // Not on github because too large
#define GRAPH_NAME_5                                "graph_5_be"            // Not on github because too large

#define PARAMS_GRAPH_NAME                           GRAPH_NAME_3
#define PARAMS_EXP_NB                               1                       // 1 : single modal, 2 : static multi-modal, 3 : time-dependent 
#define PARAMS_READ_CH_FROM_FILE                    false
#define PARAMS_WRITE_CH_TO_FILE                     false                   // Only used if not 'PARAMS_READ_CH_FROM_FILE'
#define PARAMS_SEED                                 1598118811                      // -1 for random seed
#define PARAMS_QUERY_BENCHMARK                      true
#define PARAMS_QUERY_STALL                          true
#define PARAMS_PREPROCESSING_BENCHMARK              true
#define PARAMS_NB_RUNS_QUERY_BENCHMARK              1000
#define PARAMS_VIEW                                 false