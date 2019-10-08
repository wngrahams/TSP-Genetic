/*
 * tsp-random.c
 *
 * Perform a random search on the given array of points, printing results to
 * files.
 *
 */

#include <sys/types.h>  // getpid
#include <unistd.h>     // getpid

#include "tsp-random.h"

void* random_search(void* args) {
    
    struct search_args* info;
    int num_points, LT_GT;
    int *path, *best_path;
    double total_dist, new_dist;
    struct point* point_arr;
    unsigned long int num_evals = 0L;
    unsigned int rand_state;

    // get args from struct
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

    rand_state = (int)time(NULL) ^ (int)pthread_self() ^ getpid();
    shuffle_path(&path, num_points, &rand_state);

    // array to store best path found
    best_path = malloc(num_points * sizeof(int));
    CHECK_MALLOC_ERR(best_path);

    copy_path(&path, &best_path, num_points);

    // calculate initial total distance
    total_dist = 0.0;
    for (int i=0; i<num_points; i++) {
        total_dist += calc_dist( &point_arr[path[i]],
                                 &point_arr[path[MOD(i+1, num_points)]] );
    }

    // printf("Initial total distance: %.9lf\n", total_dist);

    // open file for writing fitness curve progression
    FILE *f_progression = fopen("./output/out-Random-progression.txt", "a");
    if (NULL == f_progression) {
        perror("./output/out-Random-progression.txt");
        exit(2);
    }

    fprintf(f_progression, "0 \t%lf\n", total_dist);
    num_evals++;

    // random search: randomly shuffle, see if resulting distance is 
    // shorter
    //srand((unsigned int)time(NULL));
    while (num_evals < MAX_ITER) {
        shuffle_path(&path, num_points, &rand_state);
        new_dist = 0.0;
        for (int i=0; i<num_points; i++) {
            new_dist += calc_dist( &point_arr[path[i]],
                                   &point_arr[path[MOD(i+1, num_points)]] );
        }

        // if new_dist is better than total_dist, keep the new path
        if (lt_gt(new_dist, total_dist, LT_GT)) {
            total_dist = new_dist;
            copy_path(&path, &best_path, num_points);
            
            // write to file
            fprintf(f_progression, "%lu \t%lf\n", num_evals, total_dist);
        }

        // speed this up because it's slow and we don't really care about it
        num_evals += num_points;
    }

    // print final result
    fprintf(f_progression, "%lu \t%lf\n", num_evals, total_dist);
    printf("Random Search: %.9lf\n", total_dist);

    // Write final path to a different file
    FILE *f_path = fopen("./output/out-Random-path.txt", "a");
    if (NULL == f_path) {
        perror("./output/out-Random-path.txt");
        exit(1);
    }

    // first line is total distance
    fprintf(f_path, "%lf\n", total_dist);
    for (int i=0; i<num_points; i++) {
        fprintf(f_path, "%d\n", best_path[i]);
    }

    fclose(f_progression);
    fclose(f_path);
    free(path);
    free(best_path);

    return 0;
}

