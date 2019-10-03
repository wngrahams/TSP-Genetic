/* 
 * tsp.h
 *
 */

#ifndef _TSP_H_
#define _TSP_H_

#include <math.h>    // hypot
#include <stdint.h>  // int8_t
#include <stdio.h>   // perror
#include <stdlib.h>  // rand, srand
#include <time.h>    // time

// Citation: modulo macro from https://www.lemoda.net/c/modulo-operator/
// for correct output when taking the mod of a negaitve number
#define MOD(a,b) ((((a)%(b))+(b))%(b))

#define CHECK_MALLOC_ERR(ptr) ((!check_malloc_err(ptr)) ? (exit(1)) : (1))

#define MAX_ITER 4000000
#define LESS_THAN 0
#define GREATER_THAN 1

struct point {
    double x;
    double y;
};

/*
 * Calculates the euclidean distance between two points
 */
static inline double calc_dist(const struct point* p1, const struct point* p2){
    return hypot(p2->x - p1->x, p2->y - p1->y);
}

/* 
 * This function checks if malloc() returned NULL. If it did, the program
 * prints an error message. The function returns 1 on success and 0 on failure
 */ 
static inline int8_t check_malloc_err(const void *ptr) {
    if (NULL == ptr) {
        perror("malloc() returned NULL");
        return 0;         
    } /* END if */

    return 1;
}

/*
 * Evaluates if lhs is less than or greater than rhs depending on flag passed
 */
static inline int8_t lt_gt(const double lhs, const double rhs, const int symb){
    return ((symb == LESS_THAN) ? (lhs < rhs) : (lhs > rhs));
}

/*
 * Shuffles the array of integers passed to the function
 * Citation: shuffle algorithm by Ben Pfaff
 * https://benpfaff.org/writings/clc/shuffle.html
 */
static inline void shuffle_path(int** path, const int num_points) {
    srand((unsigned int)time(NULL));
    int switch_pos, temp;
    for (int i=0; i<num_points; i++) {
        switch_pos = i + rand() / (RAND_MAX/(num_points - i) + 1);
        temp = (*path)[switch_pos];
        (*path)[switch_pos] = (*path)[i];
        (*path)[i] = temp;
    }
}

#endif /* _TSP_H_ */

