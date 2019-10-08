/*
 * tsp-rmhc.c
 *
 * Perform a Random Mutation Hill Climbing search on the given array of points,
 * printing results to files.
 *
 */

#include <sys/types.h>  // getpid
#include <unistd.h>     // get pid

#include "tsp-rmhc.h"

void* random_mutation_hill_climbing(void* args) {
    
    struct search_args* info;
    int* path;
    double total_dist, new_dist;
    int num_points, LT_GT;
    struct point* point_arr;
    int pos1, pos2; // locus1, locus2, min, max;
    unsigned long int num_evals = 0L;
    unsigned int rand_state;

    info = (struct search_args*)args;
    point_arr = *(info->points);
    num_points = info->num_points;
    LT_GT = info->LT_GT;

    // allocate array for path
    path = malloc(num_points * sizeof(int));
    CHECK_MALLOC_ERR(path);

    // first fill array sequentially, then shuffle
    for (int i=0; i<num_points; i++) {
        path[i] = i;
    }

    rand_state = (int)time(NULL) ^ getpid() ^ (int)pthread_self();
    shuffle_path(&path, num_points, &rand_state);

    // calculate initial total distance
    total_dist = 0.0;
    for (int i=0; i<num_points; i++) {
        total_dist += calc_dist( &point_arr[path[i]],
                                 &point_arr[path[MOD(i+1, num_points)]] );
    }

    // printf("Initial total distance: %.9lf\n", total_dist);

    // open file for writing fitness curve progression
    FILE *f_progression = fopen("./output/out-RMHC-progression.txt", "a");
    if (NULL == f_progression) {
        perror("./output/out-RMHC-progression.txt");
        exit(2);
    }

    fprintf(f_progression, "0 \t%lf\n", total_dist);
    num_evals++;

    // Random Mutation Hill Climbing search: perform a hill climbing search, 
    // but each iteration introduce a random mutation by reversing the path
    // between two random points
    //srand((unsigned int)time(NULL));
    while (num_evals < MAX_ITER) {

        new_dist = total_dist;

        // choose two positions in the path randomly (the loci of the mutation)
        /*locus1 = rand_r(&rand_state) % num_points;
        do {
            locus2 = rand_r(&rand_state) % num_points;
        } while (locus2 == locus1);

        if (locus1 < locus2) {
            min = locus1;
            max = locus2;
        }
        else {
            min = locus2;
            max = locus1;
        }

        // subtract distance caused by original order
        new_dist = total_dist;
        new_dist -= (  calc_dist(&point_arr[path[MOD(min-1, num_points)]],
                                 &point_arr[path[min]])
                     + calc_dist(&point_arr[path[max]],
                                 &point_arr[path[MOD(max+1, num_points)]])  );
        
        // mutate at the loci
        mutate_flip(&path, num_points, min, max);

        // add the distance caused by the new order
        new_dist += (  calc_dist(&point_arr[path[MOD(min-1, num_points)]],
                                 &point_arr[path[max]])
                     + calc_dist(&point_arr[path[min]],
                                 &point_arr[path[MOD(max+1, num_points)]])  );
           
        */
        // choose two positions to swap
        pos1 = rand_r(&rand_state) % num_points;
        do {
            pos2 = rand_r(&rand_state) % num_points;
        } while (pos2 == pos1);

        // subtract distance caused by the original placement of these
        // two points
        new_dist -= ( calc_dist(&point_arr[path[MOD(pos1-1, num_points)]],
                                &point_arr[path[pos1]])
                      + calc_dist(&point_arr[path[pos1]],
                                  &point_arr[path[MOD(pos1+1, num_points)]])
                      + calc_dist(&point_arr[path[MOD(pos2-1, num_points)]],
                                  &point_arr[path[pos2]])
                      + calc_dist(&point_arr[path[pos2]],
                                  &point_arr[path[MOD(pos2+1, num_points)]]) );

        // swap the two points
        mutate_swap(&path, num_points, pos1, pos2);

        // add distance caused by the new placement of these two points
        new_dist += ( calc_dist(&point_arr[path[MOD(pos1-1, num_points)]],
                                &point_arr[path[pos1]])
                      + calc_dist(&point_arr[path[pos1]],
                                &point_arr[path[MOD(pos1+1, num_points)]])
                      + calc_dist(&point_arr[path[MOD(pos2-1, num_points)]],
                                  &point_arr[path[pos2]])
                      + calc_dist(&point_arr[path[pos2]],
                                  &point_arr[path[MOD(pos2+1, num_points)]]) );

        // if new_dist is better than total_dist, keep the swapped points
        if (lt_gt(new_dist, total_dist, LT_GT)) {
            total_dist = new_dist;
            
            // write to file
            fprintf(f_progression, "%lu \t%lf\n", num_evals, total_dist);
        }
        else {
            // un-mutate in reverse order (swap then flip)
            mutate_swap(&path, num_points, pos1, pos2);
            //mutate_flip(&path, num_points, min, max); 
        }

        num_evals++;
    }

    // print final result
    fprintf(f_progression, "%lu \t%lf\n", num_evals, total_dist);
    printf("Random Mutation Hill Climbing: %.9lf\n", total_dist);

    // Write final path to a different file
    FILE *f_path = fopen("./output/out-RMHC-path.txt", "a");
    if (NULL == f_path) {
        perror("./output/out-RMHC-path.txt");
        exit(1);
    }

    // first line is total distance
    fprintf(f_path, "%lf\n", total_dist);
    for (int i=0; i<num_points; i++) {
        fprintf(f_path, "%d\n", path[i]);
    }

    fclose(f_progression);
    fclose(f_path);
    free(path);

    return 0;
}

