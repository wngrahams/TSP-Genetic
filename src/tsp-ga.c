/*
 * tsp-ga.c
 *
 * Use a genetic algoritihm to find the optimal path among the points given, 
 * printing results to files.
 *
 */

#include <sys/types.h>  // getpid
#include <unistd.h>     // getpid

#include "tsp-ga.h"

#define POP_SIZE 50

struct fill_array_args {
    int*** pop_ptr;
    int idx;
    int num_points;
};

void* genetic_algorithm(void* args) {

    struct search_args* info;
    struct point* point_arr;
    int num_points, LT_GT;
    int** population;
    pthread_t workers[POP_SIZE];
    
    info = (struct search_args*)args;
    point_arr = *(info->points);
    num_points = info->num_points;
    LT_GT = info->LT_GT;

    // allocate array to hold population and their children
    population = malloc(POP_SIZE * 2 * sizeof(int*));
    CHECK_MALLOC_ERR(population);

    // start with a population of random paths
    for (int i=0; i<POP_SIZE; i++) {
        struct fill_array_args* arg = malloc(sizeof(struct fill_array_args));
        CHECK_MALLOC_ERR(arg);
        arg->pop_ptr = &population;
        arg->idx = i;
        arg->num_points = num_points;
        
        pthread_create(&workers[i], NULL, fill_rand_array, (void*)arg);
    }

    // wait for threads to return and free memory
    for (int i=0; i<POP_SIZE; i++) {
        struct fill_array_args* ptr;
        pthread_join(workers[i], (void**)&ptr);
        free(ptr);
    }

    // free population array
    for (int i=0; i<POP_SIZE*2; i++) {
        free[i];
    }

    return 0;
}

void* fill_rand_array(void* args) {

    struct fill_array_args* info;
    int** pop;
    int idx, num_points;
    int* path;
    unsigned int rand_state;

    info = (struct fill_array_args*)args;
    pop = *(info->pop_ptr);
    idx = info->idx;
    num_points = info->num_points;

    path = malloc(num_points * sizeof(int));

    // first fill array sequentially, then shuffle
    for (int i=0; i<num_points; i++) {
        path[i] = i;
    }

    rand_state = (int)time(NULL) ^ getpid() ^ (int)pthread_self();
    shuffle_path(&path, num_points, &rand_state);

    // put path in population array
    pop[idx] = path;
    
    pthread_exit(info);
}

