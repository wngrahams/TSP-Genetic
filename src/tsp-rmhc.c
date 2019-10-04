/*
 * tsp-rmhc.c
 *
 * Perform a Random Mutation Hill Climbing search on the given array of points,
 * printing results to files
 *
 */

#include "tsp-rmhc.h"

void random_mutation_hill_climbing(struct point** points,
                                   const int num_points,
                                   const int LT_GT) {

    int *best_eval;
    double best_dist;
    unsigned long int num_evals;

    struct point* point_arr = *points;

    // allocate array for best_eval
    best_eval = malloc(num_points * sizeof(int));
    CHECK_MALLOC_ERR(best_eval);

    // first fill array sequentially, then shuffle so that we start with a
    // random path
    for (int i=0; i<num_points; i++) {
        best_eval[i] = i;
    }

    shuffle_path(&best_eval, num_points);
    
    // calculate initial total distance
    best_dist = 0.0;
    for (int i=0; i<num_points; i++) {
        best_dist += calc_dist( &point_arr[best_eval[i]],
                                &point_arr[best_eval[MOD(i+1, num_points)]] );
    }

    printf("Initial total distance: %.9lf\n", best_dist);

    // open file for writing fitness curve progression
    FILE *f_progression = fopen("./output/out-RMHC-progression.txt", "a");
    if (NULL == f_progression) {
        perror("./output/out-RMHC-progression.txt");
        exit(2);
    }

    fprintf(f_progression, "0\t%lf\n", best_dist);
    num_evals = 1L;

    // Random-Mutation Hill Climbing: for each iteration, choose two random 
    // indicies. The path between those two indicies will be flipped (this is 
    // the mutation.
    
    while (num_evals < MAX_ITER) {
        
        // randomly choose 
    }
}
