/*
 * tsp-ga.c
 *
 * Use a genetic algoritihm to find the optimal path among the points given, 
 * printing results to files.
 *
 */

#include <sys/types.h>  // getpid
#include <unistd.h>     // getpid
#include <string.h>     // memset

#include "tsp-ga.h"

#define POP_SIZE 50
#define NUM_ELITE 10
#define CROSSOVER_RATE 0.90
#define MUTATION_RATE 0.005
#define INSERTION_SORT_THRESHOLD 7

void _merge_indiv(struct indiv***, const int, const int, const int, const int);

void* genetic_algorithm(void* args) {

    struct search_args* info;
    struct point* point_arr;
    int num_points, LT_GT;
    int** population;
    int** children;
    double max_cdf;
    double* cdf;
    struct indiv **pop_indiv, **child_indiv;
    unsigned long int num_evals = 0L;

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

    // wait for threads to return
    for (int i=0; i<POP_SIZE; i++) {
        pthread_join(workers[i], NULL); 
    }
    

        // take NUM_ELITE elite children directly from population
        for (int i=0; i<NUM_ELITE; i++) {
            struct indiv* elite_child_indiv = pop_indiv[i];
            int* elite_child = malloc(num_points * sizeof(int));
            CHECK_MALLOC_ERR(elite_child);
            // copy elite child from population to children
            for (int j=0; j<num_points; j++) {
                elite_child[j] = population[elite_child_indiv->idx][j];
            }

            // add it to children array
            children[i] = elite_child;

            // add a coresponding struct indiv to child_indiv array
            struct indiv* c_indiv = malloc(sizeof(struct indiv));
            c_indiv->idx = i;
            c_indiv->fitness = elite_child_indiv->fitness;
            child_indiv[i] = c_indiv;
        }

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

        // sort the array of individuals
        mergesort_individuals(&pop_indiv, 0, POP_SIZE-1, LT_GT);

        // select the rest of the children by drawing from the population (with 
        // replacement) based on the cdf of their relative fitnesses
        for (int i=NUM_ELITE; i<POP_SIZE; i++) {
            struct ga_args* rank_selection_info = malloc(sizeof(struct ga_args));
            CHECK_MALLOC_ERR(rank_selection_info);
            rank_selection_info->point_arr = point_arr;
            rank_selection_info->population = &population;
            rank_selection_info->pop_indiv = &pop_indiv;
            rank_selection_info->children = &children;
            rank_selection_info->child_indiv = &child_indiv;
            rank_selection_info->num_points = num_points;
            rank_selection_info->idx = i;
            rank_selection_info->cdf = cdf;
            rank_selection_info->max_cdf = &max_cdf;

            pthread_create(&workers[i], 
                           NULL, 
                           rank_selection_ga, 
                           (void*)rank_selection_info);
        } 

        // wait for threads to return
        for (int i=NUM_ELITE; i<POP_SIZE; i++) {
            pthread_join(workers[i], NULL);
        }

        // delete old population, transfer children to new population
        for (int i=0; i<POP_SIZE; i++) {
            free(population[i]);
            free(pop_indiv[i]);

            population[i] = children[i];
            pop_indiv[i] = child_indiv[i];

            children[i] = NULL;
            child_indiv[i] = NULL;

            children[i] = malloc(num_points * sizeof(int));
            child_indiv[i] = malloc(sizeof(struct indiv));

            CHECK_MALLOC_ERR(children[i]);
            CHECK_MALLOC_ERR(child_indiv[i]);
        }
    
    // free memory:
    for (int i=0; i<POP_SIZE; i++) {
        free(population[i]);
        free(children[i]);
        free(pop_indiv[i]);
        free(child_indiv[i]);
    }

    free(population);
    free(children);
    free(pop_indiv);
    free(child_indiv);
    free(cdf);

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

		while (j>=l && lt_gt(key, (*a)[j]->fitness, LT_GT)) {
            copy_indiv((*a)[j], (*a)[j+1]);
			j--;
		}
        copy_indiv(temp, (*a)[j+1]);
	}

    free(temp);
}

/*
 * binary search; maximum r = lenth of array - 1
 */
int binary_search_cdf(double** cdf, 
                      const int l, 
                      const int r, 
                      const double val) {
    int mid;        
    /*if (val < 0.71 && val > 0.69) {
        printf("val: %f, l: %d, r: %d\n", val, l, r);
        printf("searching in here:\n");
        for (int i=l; i<=r; i++) {
            printf("%f, ", (*cdf)[i]);
        }
        printf("\n");
    }*/

    if (r >= l) { 
        mid = l + (r - l)/2; 

        //printf("mid: %d\n", mid);
        if (mid > l) {
            //printf("369\n");
            if (val == (*cdf)[mid] || (val < (*cdf)[mid] && val > (*cdf)[mid-1]))
                 return mid;
        }
        else if (mid == l) {
            //printf("373\n");
            if (val <= (*cdf)[mid]) return mid;
        }
        
        // search left
        //printf("378\n");
        if (val < (*cdf)[mid]) return binary_search_cdf(cdf, l, mid-1, val);

        // search right
        //printf("382\n");
        return binary_search_cdf(cdf, mid+1, r, val);
    }

    return -1;
}

/*
 * performs crossover of two parents according to the "partially-mapped 
 * crossover function" (David E. Goldberg and Robert Lingle, Jr.), then 
 * randomly chooses one child to keep and one to discard
 */
void crossover_pmx(int** p1, int** p2, int** c1, int** c2, 
                   const int num_points) {
    unsigned int rand_state;
    int *map1, *map2;
    int *parent1, *parent2, *child1, *child2;
    int a, b, start, temp, idx, counter;

    // child2 might need to actually be int**, not sure if I can jsut 
    // return &child2 (pretty sure I can though)
    // might need to undo this and just dereference every time
    parent1 = *p1;
    parent2 = *p2;
    child1 = *c1;
    child2 = *c2;
    
    rand_state = (int)time(NULL) ^ getpid() ^ (int)pthread_self();
    // two random numbers between 1 and (num_points-1), inclusive
    a = rand_r(&rand_state) % (num_points-1) + 1; 
    do {
        b = rand_r(&rand_state) % (num_points-1) + 1;
    } while (b == a);
    if (b < a) {
        temp = b;
        b = a;
        a = temp;
    }

    printf("a: %d, b: %d\n", a, b);

    //child2 = malloc(num_points * sizeof(int));
    map1 = malloc(num_points * sizeof(int));
    map2 = malloc(num_points * sizeof(int));
    //CHECK_MALLOC_ERR(child2);
    CHECK_MALLOC_ERR(map1);
    CHECK_MALLOC_ERR(map2);

    // initialize each spot in the maps to -1
    memset(map1, -1, num_points * sizeof(int));
    memset(map2, -1, num_points * sizeof(int));

    // copy and swap critical portions of the parents that will remain 
    // unchanged to the children
    for (int i=a; i<b; i++) {
        child1[i] = parent2[i];
        child2[i] = parent1[i];

        // map the unchanged child values to each other so that we can know
        // what's already in the array when we fill in the rest
        map1[child1[i]] = child2[i];
        map2[child2[i]] = child1[i];
    }

    // fill in the rest of the values: try to take directly from the original
    // parent, but use the map to figure out what to take instead if we cannot
    for (int i=0; i<num_points; i++) {
        if (i<a || i>=b) {
            if (map1[parent1[i]] == -1) {
                // p1[i] is not in c1 yet, so just put it in
                child1[i] = parent1[i];
            } 
            else {
                // p1[i] already in c1, so use the map array to find something
                // else to go in c1[i]
                start = map1[parent1[i]];
                temp = start;
                counter = 0;
                while (temp != -1) {
                    idx = temp;
                    temp = map1[idx];
                    
                    if (temp == start || counter >= num_points) {
                        // we hit a cycle, just search for the next -1
                        int j=0;
                        while (map1[j] != -1) {
                            j++;
                        }

                        idx = j;
                        temp = map1[j];
                        break;
                    }

                    counter++;
                }

                child1[i] = idx;
            }

            // do the same for c2:
            if (map2[parent2[i]] == -1) {
                child2[i] = parent2[i];
            }
            else {
                start = map2[parent2[i]];
                temp = start;
                counter = 0;
                while (temp != -1) {
                    idx = temp;
                    temp = map2[idx];
                    
                    if (temp == start || counter >= num_points) {
                        int j=0;
                        while (map2[j] != -1) {
                            j++;
                        }

                        idx = j;
                        temp = map2[j];
                        break;
                    }

                    counter++;
                }

                child2[i] = idx;
            }

            // fill in map
            map1[child1[i]] = child2[i];
            map2[child2[i]] = child1[i];
        }
    }

    // free the rest of the stuff
    free(map1);
    free(map2);
}

/*
 * Given a population pre-sorted by fitness, this function performs rank
 * selection, then performs pmx crossover to make a child, and then potentially
 * performs mutation in order to make the next generation.
 */ 
void* rank_selection_ga(void* args) {

    unsigned int rand_state;
	int select_rand, indiv1_idx, indiv2_idx, cross_rand, child_rand, mutate_rand;
	int parent1_idx, parent2_idx, swap_point;
    double select_draw, cross_draw, child_dist, mutate_draw;
	int *parent1_path, *parent2_path, *child1, *child2;
	struct indiv *new_child_indiv;

    struct ga_args* info = (struct ga_args*)args;
    struct point* point_arr = info->point_arr;
    int*** population = info->population;
    struct indiv*** pop_indiv = info->pop_indiv;
    int*** children = info->children;
    struct indiv*** child_indiv = info->child_indiv;
    int num_points = info->num_points;
    int i = info->idx;
    double* cdf = info->cdf;
    double max_cdf = *(info->max_cdf);

    rand_state = (int)time(NULL) ^ getpid() ^ (int)pthread_self();
     //   for (int i=NUM_ELITE; i<POP_SIZE; i++) {

        select_rand = rand_r(&rand_state) % (int)(max_cdf * POP_SIZE * POP_SIZE);
        select_draw = (0.0+select_rand)/(0.0+(POP_SIZE*POP_SIZE));
        indiv1_idx = binary_search_cdf(&cdf, 0, POP_SIZE-1, select_draw);
        // this just multiplies by pop_size^2 so that our draw is a valid 
        // integer, then divides by the same thing to make the draw between 
        // 0 and 1
        do {
            select_rand = rand_r(&rand_state) 
                        % (int)(max_cdf * POP_SIZE * POP_SIZE);
            select_draw = (0.0+select_rand)/(0.0+(POP_SIZE*POP_SIZE));
            indiv2_idx = binary_search_cdf(&cdf, 0, POP_SIZE-1, select_draw);
        } while (indiv2_idx == indiv1_idx);

        parent1_idx = (*pop_indiv)[indiv1_idx]->idx;
        parent2_idx = (*pop_indiv)[indiv2_idx]->idx;

        parent1_path = (*population)[parent1_idx];
        parent2_path = (*population)[parent2_idx];

        // now we have the two parents, do crossover
        cross_rand = rand_r(&rand_state) % (100);
        cross_draw = (cross_rand + 0.0)/100;

        // malloc children 
        child1 = malloc(num_points * sizeof(int));
        child2 = malloc(num_points * sizeof(int));
        CHECK_MALLOC_ERR(child1);
        CHECK_MALLOC_ERR(child2);        

        if (cross_draw < CROSSOVER_RATE) {
            crossover_pmx(&parent1_path, &parent2_path, 
                          &child1, &child2, num_points);
        }
        else {
            // no crossover, parents just become children
            for (int j=0; j<num_points; j++) {
                child1[j] = parent1_path[j];
                child2[j] = parent2_path[j];
            }
        }

        // randomly choose one of the two children to add to the children array
        child_rand = rand_r(&rand_state) % 2;
        if (child_rand == 0) {
            (*children)[i] = child1;

            // calculate distance and add to child_indiv array
            child_dist = 0.0;
            for (int j=0; j<num_points; j++) {
                child_dist += 
                    calc_dist( &point_arr[child1[j]],
                               &point_arr[child1[MOD(j+1, num_points)]] );
            }

            new_child_indiv = malloc(sizeof(struct indiv));
            CHECK_MALLOC_ERR(new_child_indiv);
            new_child_indiv->fitness = child_dist;
            new_child_indiv->idx = i;

            (*child_indiv)[i] = new_child_indiv;

            free(child2);
        }
        else {
            (*children)[i] = child2;

            // calculate distance and add to child_indiv array
            child_dist = 0.0;
            for (int j=0; j<num_points; j++) {
                child_dist += 
                    calc_dist( &point_arr[child2[j]],
                               &point_arr[child2[MOD(j+1, num_points)]] );
            }

            new_child_indiv = malloc(sizeof(struct indiv));
            CHECK_MALLOC_ERR(new_child_indiv);
            new_child_indiv->fitness = child_dist;
            new_child_indiv->idx = i;

            (*child_indiv)[i] = new_child_indiv;


            free(child1);
        }

        // mutate
        for (int j=0; j<num_points; j++) {
            mutate_rand = rand_r(&rand_state) % (1000);
            mutate_draw = (mutate_rand + 0.0)/1000;

            if (mutate_draw < MUTATION_RATE) {

                do {
                    swap_point = rand_r(&rand_state) % num_points;
                } while (swap_point == j);
                mutate_swap(((*children)+i), num_points, j, swap_point);
            }
        }

    // free struct that was mallocd to send args to this function
    free(info);
    
    return 0;
}

