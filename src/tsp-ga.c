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

#define POP_SIZE 10
#define NUM_ELITE 10
#define CROSSOVER_RATE 0.85
#define MUTATION_RATE 0.005
#define INSERTION_SORT_THRESHOLD -1

void _merge_indiv(struct indiv***, const int, const int, const int, const int);

void* genetic_algorithm(void* args) {

    struct search_args* info;
    struct point* point_arr;
    int num_points, LT_GT;
    int** population;
    int** children;
    //int* lengths;
    double* cdf;
    struct indiv** pop_indiv;

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

    // allocate array to hold cdf for random draws from population 
    cdf = malloc(POP_SIZE * sizeof(double));
    CHECK_MALLOC_ERR(cdf);
    for (int i=0; i<POP_SIZE; i++) {
        cdf[i] = (POP_SIZE + 0.0 + (2*i))/(2.0*POP_SIZE*POP_SIZE);
        if (i != 0) {
            cdf[i] += cdf[i-1];
        }
//        printf("%f\n", cdf[i]);
    }

	// allocate array to hold individuals representation of population
	pop_indiv = malloc(POP_SIZE * sizeof(struct indiv*));
	CHECK_MALLOC_ERR(pop_indiv);

    // allocate array to hold lengths of the population
    //pop_lengths = malloc(POP_SIZE * sizeof(int));
    //CHECK_MALLOC_ERR(lengths);

    // initialize priority queue
    //PQueueInitialise(&pq, POP_SIZE*2, MAX_INT, LT_GT); 

    // start with a population of random paths
    for (int i=0; i<POP_SIZE; i++) {
        struct fill_array_args* arg = malloc(sizeof(struct fill_array_args));
        CHECK_MALLOC_ERR(arg);
        arg->points = &point_arr;
        arg->pop_ptr = &population;
      //  arg->lengths = &lengths;
        arg->idx = i;
        arg->num_points = num_points;
        
        pthread_create(&workers[i], NULL, fill_rand_array, (void*)arg);
    }

    // wait for threads to return, add to indiv array
    for (int i=0; i<POP_SIZE; i++) {
        struct indiv* individual;
        pthread_join(workers[i], (void**)&individual);
        pop_indiv[i] = individual;
    }
    
    printf("BEFORE\n");
    for (int i=0; i<POP_SIZE; i++) {  
        printf("%f, ", pop_indiv[i]->fitness);
    }
    printf("\n\n");
    // sort the array of individuals
    mergesort_individuals(&pop_indiv, 0, POP_SIZE-1, LT_GT);
    printf("\nAFTER\n");
    for (int i=0; i<POP_SIZE; i++) {
        printf("%f, ", pop_indiv[i]->fitness);
    }
    printf("\n");

    /*
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

    }*/

    // free population array
    for (int i=0; i<POP_SIZE; i++) {
        free(population[i]);
    }
    free(population);
    // free pop_indiv
    for (int i=0; i<POP_SIZE; i++) {
        free(pop_indiv[i]);
    }
    free(pop_indiv);
    // free cdf array
    free(cdf);
    // TODO: free children arrays

    return 0;
}

void* fill_rand_array(void* args) {

    struct fill_array_args* info;
    struct point* point_arr;
    int** pop;
    int idx, num_points;
    int *path;
    unsigned int rand_state;
    double dist;
    struct indiv* individual;

    info = (struct fill_array_args*)args;
    point_arr = *(info->points);
    pop = *(info->pop_ptr);
    //lengths = *(info->lengths);
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
    dist = 0.0;
    for (int i=0; i<num_points; i++) {
        dist += calc_dist( &point_arr[path[i]],
                             &point_arr[path[MOD(i+1, num_points)]] );
    }

    // put length in lengths array
    //lengths[idx] = length;

    individual = malloc(sizeof(struct indiv));
    CHECK_MALLOC_ERR(individual);
    individual->idx = idx;
    individual->fitness = dist;
    
    free(info);

    pthread_exit(individual);
}

/*
uint32 get_fitness(void* item) {
    struct indiv* individual;
    double fitness;
    uint32 fitness_scaled;

    individual = (struct indiv*)item;
    fitness = individual->fitness;
    fitness_scaled = (uint32)(fitness * 10000);

    return fitness_scaled;
}*/

/*
 * Basic structure of mergesort taken from:
 * https://www.geeksforgeeks.org/c-program-for-merge-sort/
 */
void mergesort_individuals(struct indiv*** a, 
                           const int l, 
                           const int r,
                           const int LT_GT) {
    int mid;
    struct indiv** array = *a;

    if (l < r) {
        if ((r - l + 1) <= INSERTION_SORT_THRESHOLD) {
            insertionsort_individuals(&array[l], (r - l + 1), LT_GT);
        }
        mid = (l+r)/2;

        // recursively sort
        mergesort_individuals(&array, l, mid, LT_GT);
        mergesort_individuals(&array, mid+1, r, LT_GT);

        // merge
        _merge_indiv(&array, l, mid, r, LT_GT);
    }

    for (int i=l; i<=r; i++) {
        printf("%f ", array[i]->fitness);
    }
    printf("\n");

}

void _merge_indiv(struct indiv*** a, 
                  const int l, 
                  const int m, 
                  const int r,
                  const int LT_GT) {
    int i, j, k;
    int n1, n2;
    struct indiv** array = *a;

    n1 = m-l+1;
    n2 = r-m;
    struct indiv *left[n1], *right[n2];

    // copy data into temp arrays
    for (i=0; i<n1; i++) {
        left[i] = array[l+i];
    }
    for(j=0; j<n2; j++) {
        right[i] = array[m+1+j];
    }

    // merge temp arrays back into main arrays
	// merge temp arrays back into main arrays
	i = 0;
	j = 0;
	k = 0;
	while (i<n1 && j<n2) { 
        if (lt_gt(left[i]->fitness, right[i]->fitness, LT_GT)) { 
            array[k] = left[i]; 
            i++; 
        } 
        else { 
            array[k] = right[j]; 
            j++; 
        } 
        k++; 
	}

	// put in any leftovers
	while (i < n1) { 
        array[k] = left[i]; 
        i++; 
        k++; 
	} 
	while (j < n2) { 
        array[k] = right[j]; 
        j++; 
        k++; 
	} 
}

void insertionsort_individuals(struct indiv** array, 
                               const int len,
                               const int LT_GT) {
	int i, j;
	double key;
	struct indiv* temp;
	
	for (i=1; i<len; i++) {
		key = array[i]->fitness;
		temp = array[i];
		j = i - 1;

		while (j>=0 && lt_gt(key, array[j]->fitness, LT_GT)) {
			array[j+1] = array[j];
			j--;
		}
		array[j+1] = temp;		
	}
}

