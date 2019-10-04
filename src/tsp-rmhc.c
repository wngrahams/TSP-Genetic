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
    int pos1, pos2, temp;
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

    fprintf(f_progression, "0\t%lf\n", total_dist);
    num_evals++;

    // Random Mutation Hill Climbing search: perform a hill climbing search, 
    // but each iteration introduce a random mutation by swapping two random 
    // points
    //srand((unsigned int)time(NULL));
    while (num_evals < MAX_ITER) {
        // choose two positions to swap
        pos1 = rand_r(&rand_state) % num_points;
        do {
            pos2 = rand_r(&rand_state) % num_points;
        } while (pos2 == pos1);

        // subtract distance caused by the original placement of these
        // two points
        new_dist = total_dist;
        new_dist -= ( calc_dist(&point_arr[path[MOD(pos1-1, num_points)]],
                                &point_arr[path[pos1]])
                      + calc_dist(&point_arr[path[pos1]],
                                  &point_arr[path[MOD(pos1+1, num_points)]])
                      + calc_dist(&point_arr[path[MOD(pos2-1, num_points)]],
                                  &point_arr[path[pos2]])
                      + calc_dist(&point_arr[path[pos2]],
                                  &point_arr[path[MOD(pos2+1, num_points)]]) );

        // swap the two points
        temp = path[pos2];
        path[pos2] = path[pos1];
        path[pos1] = temp;

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
            fprintf(f_progression, "%lu\t%lf\n", num_evals, total_dist);
        }
        else {
            // switch them back
            temp = path[pos2];
            path[pos2] = path[pos1];
            path[pos1] = temp;
        }

        num_evals++;
    }

    // print final result
    fprintf(f_progression, "%lu\t%lf\n", num_evals, total_dist);
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

