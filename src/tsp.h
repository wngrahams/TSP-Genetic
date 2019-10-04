/* 
 * tsp.h
 *
 */

#ifndef _TSP_H_
#define _TSP_H_

#include <math.h>     // hypot
#include <pthread.h>  // pthread
#include <stdint.h>   // int8_t
#include <stdio.h>    // perror
#include <stdlib.h>   // rand, srand
#include <time.h>     // time

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

struct search_args {
    struct point** points;
    int num_points;
    int LT_GT;
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
 *
 * I added rand_r instead of rand so that it's thread safe
 */
static inline void shuffle_path(int** path, 
                                const int num_points, 
                                unsigned int* state) {
    int switch_pos, temp;
    for (int i=0; i<num_points; i++) {
        switch_pos = i + rand_r(state) / (RAND_MAX/(num_points - i) + 1);
        temp = (*path)[switch_pos];
        (*path)[switch_pos] = (*path)[i];
        (*path)[i] = temp;
    }
}

/*
 * Copies all values in src to dest. Both arrays must have size num_points.
 */
static inline void copy_path(int** src, int** dest, const int num_points) {
    for (int i=0; i<num_points; i++) {
        (*dest)[i] = (*src)[i];
    }
}

/*
 * encodes an array of indices that correspond to a path of treversal into an
 * array that represents "how much out of order" each point in the path is 
 * compared to its index. This representation allows the path to be treated as
 * a chromosome in genetic algorithms because switching two genes (items in the
 * array) can no longer result in an invalid path representation. This method
 * of representation was obtained from a paper by Göktürk Üçoluk at Middle East
 * Technical University 
 * (http://user.ceng.metu.edu.tr/~ucoluk/research/publications/tspnew.pdf)
 */
static inline void encode_path(int** path, 
                               int** chromosome, 
                               const int num_points) {
    int j;
    for (int i=0; i<num_points; i++) {
        (*chromosome)[i] = 0;
        j = 0;
        while ((*path)[j] != i) {
            if ((*path)[j] > i)
                (*chromosome)[i]++;
            j++;
        }
    }
}

/*
 * decodes an array in the chromosome representation described above back into
 * the regular path representation
 */
static inline void decode_path(int** chromosome, 
                               int** path, 
                               const int num_points) {
    int* pos = malloc(num_points * sizeof(int));
    CHECK_MALLOC_ERR(pos);
    
    for (int i=(num_points-1); i>=0; i--) {
        for (int j=i+1; j<num_points; j++) {
            if (pos[j] >= ((*chromosome)[i])) {
                pos[j]++;
            }
        }

        pos[i] = (*chromosome)[i];
    
    }

    for (int i=0; i<num_points; i++) {
       (*path)[pos[i]] = i;
    }

    free(pos);
}

static inline void mutate(int** path, 
                          const int num_points, 
                          unsigned int* state) {
    int pos1, pos2;
    int min, max;
    int temp, swap1, swap2;
    
    // choose two positions in the path randomly
    pos1 = rand_r(state) % num_points;
    do {
        pos2 = rand_r(state) % num_points;
    } while (pos2 == pos1);

    if (pos1 < pos2) {
        min = pos1;
        max = pos2;
    }
    else {
        min = pos2;
        max = pos1;
    }

    for (int i=0; i<=(max-min)/2; i++) {
        swap1 = min + i;
        swap2 = max - i;
        temp = (*path)[swap2];
        (*path)[swap2] = (*path)[swap1];
        (*path)[swap1] = temp;
    }
}

#endif /* _TSP_H_ */

