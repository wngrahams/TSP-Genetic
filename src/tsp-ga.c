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
#define NUM_ELITE 10
#define CROSSOVER_RATE 0.85
#define MUTATION_RATE 0.005
#define INSERTION_SORT_THRESHOLD 7

void _merge_indiv(struct indiv***, const int, const int, const int, const int);

void* genetic_algorithm(void* args) {

    struct search_args* info;
    struct point* point_arr;
    int num_points, LT_GT, max_cdf, temp_rand;
    int parent1_idx, parent2_idx;
    int** population;
    int** children;
    unsigned int rand_state;
    double draw;
    double* cdf;
    struct indiv **pop_indiv, **child_indiv;
    int *parent1_path, *parent2_path;

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
        cdf[i] = (3*POP_SIZE + 0.0 - (2*i))/(2.0*POP_SIZE*POP_SIZE);
        if (i != 0) {
            cdf[i] += cdf[i-1];
        }
    }
    max_cdf = cdf[POP_SIZE-1];

	// allocate array to hold individuals representation of population
	pop_indiv = malloc(POP_SIZE * sizeof(struct indiv*));
	CHECK_MALLOC_ERR(pop_indiv);

    // array for individuals representation of children
    child_indiv = malloc(POP_SIZE * sizeof(struct indiv*));
    CHECK_MALLOC_ERR(child_indiv);

    // start with a population of random paths
    for (int i=0; i<POP_SIZE; i++) {
        // this gets freed in fill_rand_array()
        struct fill_array_args* arg = malloc(sizeof(struct fill_array_args));
        CHECK_MALLOC_ERR(arg);
        arg->points = &point_arr;
        arg->pop_ptr = &population;
        arg->individuals = pop_indiv;
        arg->idx = i;
        arg->num_points = num_points;
        
        pthread_create(&workers[i], NULL, fill_rand_array, (void*)arg);
    }

    // wait for threads to return, add to indiv array
    for (int i=0; i<POP_SIZE; i++) {
        pthread_join(workers[i], NULL); 
    }
    
    // sort the array of individuals
    mergesort_individuals(&pop_indiv, 0, POP_SIZE-1, LT_GT);

    // take NUM_ELITE elite children directly from population
    for (int i=0; i<NUM_ELITE; i++) {
        struct indiv* elite_child = pop_indiv[i];
        children[i] = population[elite_child->idx];

        struct indiv* c_indiv = malloc(sizeof(struct indiv));
        c_indiv->idx = i;
        c_indiv->fitness = elite_child->fitness;
        child_indiv[i] = c_indiv;
    }

    // select the rest of the children by drawing from the population (with 
    // replacement) based on the cdf of their relative fitnesses
    rand_state = (int)time(NULL) ^ getpid() ^ (int)pthread_self();
    for (int i=NUM_ELITE; i<POP_SIZE; i++) {
        temp_rand = rand_r(&rand_state) % (max_cdf * POP_SIZE * POP_SIZE);
        draw = (0.0+temp_rand)/(0.0+(POP_SIZE*POP_SIZE));
        int indiv1_idx = binary_search_cdf(&cdf, 0, POP_SIZE-1, draw);
        int indiv2_idx;
        do {
            temp_rand = rand_r(&rand_state) % (max_cdf * POP_SIZE * POP_SIZE);
            draw = (0.0+temp_rand)/(0.0+(POP_SIZE*POP_SIZE));
            indiv2_idx = binary_search_cdf(&cdf, 0, POP_SIZE-1, draw);
        } while (indiv2_idx == indiv1_idx);

        printf("max_cdf: %f\n", max_cdf);
        printf("indiv1_idx: %d, indiv2_idx: %d\n", indiv1_idx, indiv2_idx);

        /*
        parent1_idx = pop_indiv[indiv1_idx]->idx;
        parent2_idx = pop_indiv[indiv2_idx]->idx;

        parent1_path = population[parent1_idx];
        parent2_path = population[parent2_idx];
        */
        // now we have the two parents, do crossover and then randomly 
        // choose one child to keep
    }

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
    free(children);

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
    struct indiv** individuals;
    struct indiv* individual;

    info = (struct fill_array_args*)args;
    point_arr = *(info->points);
    pop = *(info->pop_ptr);
    individuals = info->individuals;
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


    individual = malloc(sizeof(struct indiv));
    CHECK_MALLOC_ERR(individual);
    individual->idx = idx;
    individual->fitness = dist;

    // put individual in individuals array
    individuals[idx] = individual;
    
    free(info);

    return 0;
}

/*
 * Basic structure of mergesort taken from:
 * https://www.geeksforgeeks.org/c-program-for-merge-sort/
 */
void mergesort_individuals(struct indiv*** a, 
                           const int l, 
                           const int r,
                           const int LT_GT) {
    int mid;

    if (l < r) {
        if ((r - l + 1) <= INSERTION_SORT_THRESHOLD) {
            insertionsort_individuals(a, l, r+1, LT_GT);
            return;
        }
        mid = (l+r)/2;

        // recursively sort
        mergesort_individuals(a, l, mid, LT_GT);
        mergesort_individuals(a, mid+1, r, LT_GT);

        // merge
        _merge_indiv(a, l, mid, r, LT_GT);
    }
}

void _merge_indiv(struct indiv*** a, 
                  const int l, 
                  const int m, 
                  const int r,
                  const int LT_GT) {
    int i, j, k;
    int n1, n2;
    struct indiv **left, **right;
    
    n1 = m-l+1;
    n2 = r-m;

    // malloc temp arrays
    left = malloc(n1 * sizeof(struct indiv*));
    right = malloc(n2 * sizeof(struct indiv*));
    CHECK_MALLOC_ERR(left);
    CHECK_MALLOC_ERR(right);

    // copy data into temp arrays
    for (i=0; i<n1; i++) {
        left[i] = malloc(sizeof(struct indiv));
        CHECK_MALLOC_ERR(left[i]);
        copy_indiv((*a)[l+i], left[i]);
    }
    for(j=0; j<n2; j++) {
        right[j] = malloc(sizeof(struct indiv));
        CHECK_MALLOC_ERR(right[j]);
        copy_indiv((*a)[m+1+j], right[j]);
    }

    // merge temp arrays back into main arrays
	// merge temp arrays back into main arrays
	i = 0;
	j = 0;
	k = l;
	while (i<n1 && j<n2) { 
        if (lt_gt(left[i]->fitness, right[j]->fitness, LT_GT)) { 
            copy_indiv(left[i], (*a)[k]);         
            i++; 
        } 
        else {
            copy_indiv(right[j], (*a)[k]); 
            j++; 
        } 
        k++; 
	}

	// put in any leftovers
	while (i < n1) {
        copy_indiv(left[i], (*a)[k]); 
        i++; 
        k++; 
	} 
	while (j < n2) {
        copy_indiv(right[j], (*a)[k]); 
        j++; 
        k++; 
	}

    // free temp arrays:
    for (i=0; i<n1; i++) {
        free(left[i]);
    }
    free(left);
    for (j=0; j<n2; j++) {
        free(right[j]);
    }
    free(right);
}

void insertionsort_individuals(struct indiv*** a, 
                               const int l, const int r,
                               const int LT_GT) {
	int i, j;
	double key;
	struct indiv* temp;

    temp = malloc(sizeof(struct indiv));
    CHECK_MALLOC_ERR(temp);

	for (i=l+1; i<r; i++) {
		key = (*a)[i]->fitness;
		copy_indiv((*a)[i], temp);
		j = i - 1;

		while (j>=0 && lt_gt(key, (*a)[j]->fitness, LT_GT)) {
            copy_indiv((*a)[j], (*a)[j+1]);
			j--;
		}
        copy_indiv(temp, (*a)[j+1]);
	}

    free(temp);
}

/*
 * binary search; r = lenth of array - 1
 */
int binary_search_cdf(double** cdf, 
                      const int l, 
                      const int r, 
                      const double val) {
    
    int mid;

    if (r >= l) { 
        mid = l + (r - l)/2; 

        if (mid > 0) {
            if (val < (*cdf)[mid] && val > (*cdf)[mid-1]) return mid;
        }
        else if (mid == 0) {
            if (val < (*cdf)[mid]) return mid;
        }
        
        // search left
        if (val < (*cdf)[mid]) return binary_search_cdf(cdf, l, mid-1, val);

        // search right
        return binary_search_cdf(cdf, mid+1, r, val);
    }

    return -1;
}

