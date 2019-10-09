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
    struct indiv** individuals;
    int idx;
    int num_points;
};

struct ga_args {
    struct point* point_arr;
    
    int*** population;
    struct indiv*** pop_indiv;
    int*** children;
    struct indiv*** child_indiv;
    
    int num_points;
    int idx;

    int LT_GT;
    
    double* cdf;
    double* max_cdf;

    int *best_dist;
    int *best_dist_idx;
};

static inline void copy_indiv(struct indiv* src, struct indiv* dest) {
    dest->idx = src->idx;
    dest->fitness = src->fitness;
}

void* genetic_algorithm(void*);

void* fill_rand_array(void*);

void mergesort_individuals(struct indiv***, const int, const int, const int);
void insertionsort_individuals(struct indiv***, const int, const int, const int);
int binary_search_cdf(double**, const int, const int, const double);
void crossover_pmx(int**, int**, int**, int**, const int);
void* rank_selection_ga(void*);
void* tournament_selection_ga(void*);

#endif /* _TSP_GA_H_ */

