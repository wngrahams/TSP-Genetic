/*
 * tsp-sahc.c
 *
 * Perform a Steepest-Ascent Hill Climbing search on the given array of points,
 * printing results to files.
 *
 */

#include "tsp-sahc.h"

void steepest_ascent_hill_climbing(struct point** points,
                   const int num_points,
                   const int LT_GT) {

    int *current_path, *hilltop;
    double current_dist, hilltop_dist, best_swap_dist, new_dist;
    unsigned long int num_evals;
    int best_swap_pos, temp;

    struct point* point_arr = *points;

    // allocate array for current_path
    current_path = malloc(num_points * sizeof(int));
    CHECK_MALLOC_ERR(current_path);

    // first fill array sequentially, then shuffle so that we start with a
    // random path
    for (int i=0; i<num_points; i++) {
        current_path[i] = i;
    }

    shuffle_path(&current_path, num_points);

    // calculate initial total distance
    current_dist = 0.0;
    for (int i=0; i<num_points; i++) {
        current_dist += calc_dist(&point_arr[current_path[i]],
                                &point_arr[current_path[MOD(i+1, num_points)]]); 
    }

    printf("Initial total distance: %.9lf\n", current_dist);

    // open file for writing fitness curve progression
    FILE *f_progression = fopen("./output/out-SAHC-progression.txt", "a");
    if (NULL == f_progression) {
        perror("./output/out-SAHC-progression.txt");
        exit(2);
    }

    fprintf(f_progression, "0\t%lf\n", current_dist);
    num_evals = 1L;

    // steepest-ascent hill climbing: systematically switch each point
    // starting from left to right; set current_string to the one with the
    // highest fitness increase, then repeat. If there was no fitness increase,
    // then save current_string as a hilltop, start with a new random string.
    // Return highest hilltop
    
    // allocate array where we can save running best hilltop
    hilltop = malloc(num_points * sizeof(int));
    CHECK_MALLOC_ERR(hilltop);

    copy_path(&current_path, &hilltop, num_points);
    hilltop_dist = current_dist;

    while (num_evals < MAX_ITER) {

        //num_evals++;

        best_swap_pos = 0;
        best_swap_dist = current_dist;

        for (int i=1; i<num_points; i++) {

            new_dist = current_dist;
            
            // subtract distance caused by the original placement of these 
            // two points
            new_dist-=( calc_dist(&point_arr[current_path[num_points-1]],
                                  &point_arr[current_path[0]])
                      + calc_dist(&point_arr[current_path[0]],
                                  &point_arr[current_path[1]])
                      + calc_dist(&point_arr[current_path[i-1]],
                                  &point_arr[current_path[i]])
                      + calc_dist(&point_arr[current_path[i]],
                                  &point_arr[current_path[MOD(i+1,num_points)]]));

            // swap position 0 and position i
            temp = current_path[i];
            current_path[i] = current_path[0];
            current_path[0] = temp;

            // add distance caused by the new placement:
            new_dist+=( calc_dist(&point_arr[current_path[num_points-1]],
                                  &point_arr[current_path[0]])
                      + calc_dist(&point_arr[current_path[0]],
                                  &point_arr[current_path[1]])
                      + calc_dist(&point_arr[current_path[i-1]],
                                  &point_arr[current_path[i]])
                      + calc_dist(&point_arr[current_path[i]],
                                  &point_arr[current_path[MOD(i+1,num_points)]]));

            if (lt_gt(new_dist, best_swap_dist, LT_GT)) {
                best_swap_dist = new_dist;
                best_swap_pos = i;
            }

            // swap them back
            temp = current_path[i];
            current_path[i] = current_path[0];
            current_path[0] = temp;

            // write to file
            /*if (lt_gt(best_swap_dist, hilltop_dist, LT_GT))
                fprintf(f_progression, "%lu\t%lf\n", num_evals, current_dist);
            */
            // increment num_evals here so that computational cost is
            // comparable across search methods
            num_evals++;
            if (num_evals >= MAX_ITER) {
                break;
            }
        }

        if (best_swap_pos == 0) {
            // no fitness increase: save current path as a hilltop (if it's the
            // best hilltop), restart with a new random path
            if (lt_gt(current_dist, hilltop_dist, LT_GT)) {
                hilltop_dist = current_dist;
                copy_path(&current_path, &hilltop, num_points);
                
                // write to file
                fprintf(f_progression, "%lu\t%lf\n", num_evals, hilltop_dist);
            }

            shuffle_path(&current_path, num_points);
            current_dist = 0.0;
            for (int i=0; i<num_points; i++) {
                current_dist+=calc_dist(&point_arr[current_path[i]],
                                &point_arr[current_path[MOD(i+1,num_points)]]);
            }
        }
        else {
            // swap at the best swap pos
            temp = current_path[best_swap_pos];
            current_path[best_swap_pos] = current_path[0];
            current_path[0] = temp;

            current_dist = best_swap_dist;

            // write to file
            if (lt_gt(current_dist, hilltop_dist, LT_GT))
                fprintf(f_progression, "%lu\t%lf\n", num_evals, current_dist);
        }


    }

    // print best hilltop to file
    fprintf(f_progression, "%lu\t%lf\n", num_evals, hilltop_dist);
    printf("Final distance: %.9lf\n", hilltop_dist);

    // print final path
    FILE *f_path = fopen("./output/out-SAHC-path.txt", "a");
    if (NULL == f_path) {
        perror("./output/out-SAHC-path.txt");
        exit(2);
    }

    // first line is total distance
    fprintf(f_path, "%lf\n", hilltop_dist);
    for (int i=0; i<num_points; i++) {
        fprintf(f_path, "%d\n", hilltop[i]);
    }

    fclose(f_progression);
    fclose(f_path);
    free(current_path);
    free(hilltop);
}

