#!/bin/bash

################################
# HNSW construction parameters #
################################

M="16"                # Min number of edges per point
efConstruction="300"  # Max number of candidate vertices in priority queue to observe during construction

###################
# Data parameters #
###################

nb="1000000"          # Number of base vectors
nt="100000"           # Number of learn vectors
nc="100000"           # Number of groups
nq="10000"            # Number of queries
ngt="100"             # Number of groundtruth neighbours per query

d="128"               # Vector dimension

#####################
# Search parameters #
#####################

k="100"                 # Number of the closest vertices to search
efSearch="300"         # Max number of candidate vertices in priority queue to observe during searching

#########
# Paths #
#########

path_data="${PWD}/data/sift"
path_model="${PWD}/models/sift"

path_base="${path_data}/sift_base.fvecs"
path_learn="${path_data}/sift_learn.fvecs"
path_gt="${path_data}/sift_groundtruth.ivecs"
path_q="${path_data}/sift_query.fvecs" #query
path_group_idxs="${path_data}/idxs_100k.ivecs"

path_index="${path_model}/group_hnsw_M${M}_ef${efConstruction}.index"

#######
# Run #
#######
${PWD}/bin/test_sift -M ${M} \
                     -efConstruction ${efConstruction} \
                     -nb ${nb} \
                     -nt ${nt} \
                     -nc ${nc} \
                     -nq ${nq} \
                     -ngt ${ngt} \
                     -d ${d} \
                     -k ${k} \
                     -efSearch ${efSearch} \
                     -path_base ${path_base} \
                     -path_learn ${path_learn} \
                     -path_gt ${path_gt} \
                     -path_q ${path_q} \
                     -path_group_idxs ${path_group_idxs} \
                     -path_index ${path_index}