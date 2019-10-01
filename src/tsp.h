/* 
 * tsp.h
 *
 */

#ifndef _TSP_H_
#define _TSP_H_

#include <math.h>    // hypot
#include <stdint.h>  // int8_t
#include <stdio.h>   // perror

// Citation: modulo macro from https://www.lemoda.net/c/modulo-operator/
// for correct output when taking the mod of a negaitve number
#define MOD(a,b) ((((a)%(b))+(b))%(b))

#define MIN_ITER 2000000
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
int8_t check_malloc_err(const void *ptr) {
	if (NULL == ptr) {
		perror("malloc() returned NULL");
		return 0;         
	} /* END if */

	return 1;
}

/*
 * Evaluates if lhs is less than or greater than rhs depending on flag passed
 */
static inline int8_t lt_gt(const double lhs, const double rhs, const int symb) {
	return ((symb == LESS_THAN) ? (lhs < rhs) : (lhs > rhs));
}

#endif /* _TSP_H_ */

