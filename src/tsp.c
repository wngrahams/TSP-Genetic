/*
 * tsp.c
 * 
 * Find solution to traveling salesperson problem using genetic algorithms
 *
 */

#include <string.h>   // strtok
#include <strings.h>  // strcasecmp

#include "tsp.h"
#include "tsp-random.h"
#include "tsp-sahc.h"
#include "tsp-rmhc.h"
#include "tsp-ga.h"

int main(int argc, char** argv) {

    // open tsp datapoints file specified in command line
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "%s\n", "usage: tsp <tsp_file> <optional: lt/gt>");
        exit(1);
    }

    int LT_GT = LESS_THAN;
    if (argc == 3 && strcasecmp(argv[2], "GT") == 0)
        LT_GT = GREATER_THAN; 

    char* filename = argv[1];
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror(filename);
        exit(1);
    }

    // determine number of points in the file
    int num_points = 0;
    char line[1024]; 
    
    while (fgets(line, sizeof(line), fp)) {
        num_points++;
    }

    printf("Number of points in the file: %d\n", num_points);

    // once we've determined the number of points in the file, malloc an
    // appropriately sized array and load the points
    struct point* point_arr = malloc(num_points * sizeof(struct point));
    CHECK_MALLOC_ERR(point_arr);
    fseek(fp, 0L, SEEK_SET);
    
    char* x_str = "";
    char* y_str = "";
    char* separators = "\t \n";
    int counter = 0;

    while (fgets(line, sizeof(line), fp)) {
        x_str = strtok(line, separators);
        y_str = strtok(NULL, separators);

        point_arr[counter].x = atof(x_str);
        point_arr[counter++].y = atof(y_str);
    }

    // make a copy of the array for each search so each search can do whatever
    // it wants and the next can start fresh
    struct point* point_arr_rand = malloc(num_points * sizeof(struct point));
    struct point* point_arr_sahc = malloc(num_points * sizeof(struct point));
    struct point* point_arr_rmhc = malloc(num_points * sizeof(struct point));
    struct point* point_arr_rsga = malloc(num_points * sizeof(struct point));
    struct point* point_arr_tsga = malloc(num_points * sizeof(struct point));
    CHECK_MALLOC_ERR(point_arr_rand);
    CHECK_MALLOC_ERR(point_arr_sahc);
    CHECK_MALLOC_ERR(point_arr_rmhc);
    CHECK_MALLOC_ERR(point_arr_rsga);
    CHECK_MALLOC_ERR(point_arr_tsga);

    for (int i=0; i<num_points; i++) {
        point_arr_rand[i] = point_arr[i];
        point_arr_sahc[i] = point_arr[i];
        point_arr_rmhc[i] = point_arr[i];
        point_arr_rsga[i] = point_arr[i];
        point_arr_tsga[i] = point_arr[i];
    }

    struct search_args random_args = { 
        .points = &point_arr_rand, 
        .num_points = num_points, 
        .LT_GT = LT_GT 
    };

    struct search_args sahc_args = {
        .points = &point_arr_sahc,
        .num_points = num_points,
        .LT_GT = LT_GT
    };

    struct search_args rmhc_args = {
        .points = &point_arr_rmhc,
        .num_points = num_points,
        .LT_GT = LT_GT
    };

    struct search_args rsga_args = {
        .points = &point_arr_rsga,
        .num_points = num_points,
        .LT_GT = LT_GT,
        .options = RANK_SELECTION
    };

    struct search_args tsga_args = {
        .points = &point_arr_tsga,
        .num_points = num_points,
        .LT_GT = LT_GT,
        .options = TOURNAMENT_SELECTION
    };

    pthread_t thread_rand, thread_sahc, thread_rmhc, thread_rsga, thread_tsga;

    
    pthread_create(&thread_rand, NULL, random_search, (void*)&random_args);
    pthread_create(&thread_sahc, NULL, 
                   steepest_ascent_hill_climbing, (void*)&sahc_args);
    pthread_create(&thread_rmhc, NULL,
                   random_mutation_hill_climbing, (void*)&rmhc_args);
    pthread_create(&thread_rsga, NULL, genetic_algorithm, (void*)&rsga_args);
    pthread_create(&thread_tsga, NULL, genetic_algorithm, (void*)&tsga_args);

    pthread_join(thread_rand, NULL);
    pthread_join(thread_sahc, NULL);
    pthread_join(thread_rmhc, NULL);
    pthread_join(thread_rsga, NULL);
    pthread_join(thread_tsga, NULL); 

    fclose(fp);
    free(point_arr);
    free(point_arr_rand);
    free(point_arr_sahc);
    free(point_arr_rmhc);
    free(point_arr_rsga);
    free(point_arr_tsga);
    return 0;
}

