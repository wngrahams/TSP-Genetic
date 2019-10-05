/*
 * tsp-ga.h
 *
 */

#ifndef _TSP_GA_H_
#define _TSP_GA_H_

#include "tsp.h"

struct indiv {
    int idx;
    double fitness;
};

struct fill_array_args {
    struct point** points;
    int*** pop_ptr;
    int** lengths;
    int idx;
    int num_points;
};

void* genetic_algorithm(void*);

void* fill_rand_array(void*);

void mergesort_individuals(struct indiv**, const int, const int, const int);
void insertionsort_individuals(struct indiv**, const int, const int);

#endif /* _TSP_GA_H_ */
