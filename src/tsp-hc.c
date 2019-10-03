/*
 * tsp-hc.c
 *
 * Perform a Steepest-Ascent Hill Climbing search on the given array of points,
 * printing results to files.
 *
 */

#include "tsp-hc.h"

void hill_climbing(struct point** points,
                   const int num_points,
                   const int LT_GT) {

    int* current_path;

    // allocate array for current_path
    current_path = malloc(num_points * sizeof(int));
    CHECK_MALLOC_ERR(current_path);

    free(current_path);
}

