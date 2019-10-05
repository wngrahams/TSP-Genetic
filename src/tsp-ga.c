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
#define NUM_ELITE 5
#define CROSSOVER_RATE 0.85
#define MUTATION_RATE 0.005

struct fill_array_args {
    struct point** points;
    int*** pop_ptr;
    int** lengths;
    int idx;
    int num_points;
};

struct indiv {
    int idx;  // index into the population and lengths arrays
    double fitness;  // individual's fitness
};

void* genetic_algorithm(void* args) {

    struct search_args* info;
    struct point* point_arr;
    int num_points, LT_GT;
    int** population;
    int** children;
    int* lengths;
    PQUEUE pq;
    pthread_t workers[POP_SIZE];
    
    info = (struct search_args*)args;
    point_arr = *(info->points);
    num_points = info->num_points;
    LT_GT = info->LT_GT;

    // allocate array to hold population and their children
    population = malloc(POP_SIZE * sizeof(int*));
    children = malloc(POP_SIZE * sizeof(int*));
    CHECK_MALLOC_ERR(population);
    CHECK_MALLOC_ERR(children);

    // allocate array to hold lengths of the population
    //pop_lengths = malloc(POP_SIZE * sizeof(int));
    //CHECK_MALLOC_ERR(lengths);

    // initialize priority queue
    PQueueInitialise(&pq, POP_SIZE*2, MAX_INT, LT_GT); 

    // start with a population of random paths
    for (int i=0; i<POP_SIZE; i++) {
        struct fill_array_args* arg = malloc(sizeof(struct fill_array_args));
        CHECK_MALLOC_ERR(arg);
        arg->points = &point_arr;
        arg->pop_ptr = &population;
        arg->lengths = &lengths;
        arg->idx = i;
        arg->num_points = num_points;
        
        pthread_create(&workers[i], NULL, fill_rand_array, (void*)arg);
    }

    // wait for threads to return, add indiv struct to priority queue
    for (int i=0; i<POP_SIZE; i++) {
        struct indiv* individual;
        pthread_join(workers[i], &individual);
        if (!PQueuePush(&pq, individual, get_fitness)) {
            perror("Failed to add to queue");
            exit(3);
        }        
    }

    // get the elite children from the priority queue, then put them back in
    struct indiv holder[NUM_ELITE];
    for (int i=0; i<NUM_ELITE; i++) {
        struct indiv* individual;
        individual = PQueuePop(&pq, get_fitness);
        children[i] = path[individual->idx];
        holder[i] = individual;
    }
    // put them back in
    for (int i=0; i<NUM_ELITE; i++) {
        if (!PQueuePush(&pq, holder[i], get_fitness)) {
            perror("Failed to add to queue");
            exit(3);
        }
    }

    // make the rest of the children
    for (int i=0; i<(POP_SIZE-NUM_ELITE); i++) {

    }

    // free population array
    for (int i=0; i<POP_SIZE; i++) {
        free(population[i]);
    }
    free(population);
    //free(lengths);

    return 0;
}

void* fill_rand_array(void* args) {

    struct fill_array_args* info;
    struct point* point_arr;
    int** pop;
    int idx, num_points;
    int *path, *lengths;
    unsigned int rand_state;
    double dist;
    struct indiv* individual;

    info = (struct fill_array_args*)args;
    point_arr = *(info->points):
    pop = *(info->pop_ptr);
    lengths = *(info->lengths):
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

    // calculate distance (fitness) of path
    length = 0.0;
    for (i=0; i<num_points; i++) {
        length += calc_dist( &point_arr[path[i]],
                             &point_arr[path[MOD(i+1, num_points)]] );
    }

    // put length in lengths array
    //lengths[idx] = length;

    individual = malloc(sizeof(struct indiv));
    individual->idx = idx;
    individual->fitness = length;
    
    free(info);

    pthread_exit(individual);
}

uint32 get_fitness(void* item) {
    struct indiv* individual;
    double fitness;
    uint32 fitness_scaled;

    individual = (struct indiv*)item;
    fitness = individual->fitness;
    fitness_scaled = (uint32)(fitness * 10000);

    return fitness_scaled;
}

