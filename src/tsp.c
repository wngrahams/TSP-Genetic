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
    struct point* point_arr_ga   = malloc(num_points * sizeof(struct point));
    CHECK_MALLOC_ERR(point_arr_rand);
    CHECK_MALLOC_ERR(point_arr_sahc);
    CHECK_MALLOC_ERR(point_arr_rmhc);
    CHECK_MALLOC_ERR(point_arr_ga  );

    for (int i=0; i<num_points; i++) {
        point_arr_rand[i] = point_arr[i];
        point_arr_sahc[i] = point_arr[i];
        point_arr_rmhc[i] = point_arr[i];
        point_arr_ga  [i] = point_arr[i];
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

    struct search_args ga_args = {
        .points = &point_arr_ga,
        .num_points = num_points,
        .LT_GT = LT_GT
    };

    pthread_t thread_rand, thread_sahc, thread_rmhc, thread_ga;

    
    pthread_create(&thread_rand, NULL, random_search, (void*)&random_args);
    pthread_create(&thread_sahc, NULL, 
                   steepest_ascent_hill_climbing, (void*)&sahc_args);
    pthread_create(&thread_rmhc, NULL,
                   random_mutation_hill_climbing, (void*)&rmhc_args);
    pthread_create(&thread_ga, NULL, genetic_algorithm, (void*)&ga_args);

/*    
    printf("Random Search:\n");    
    random_search(&point_arr_rand, num_points, LT_GT);

    printf("\nSteepest-Ascent Hill Climbing\n");
    steepest_ascent_hill_climbing(&point_arr_sahc, num_points, LT_GT);

    printf("\nRandom-Mutation Hill Climbing\n");
    random_mutation_hill_climbing(&point_arr_rmhc, num_points, LT_GT);
*/    

    /*
    int a[] = {0, 3, 0, 1, 0};
    int* path = &a[0];
    int* chro = malloc(5 * sizeof(int));
    CHECK_MALLOC_ERR(chro);
    decode_path(&path, &chro, 5);
    for (int i=0; i<5; i++) {
        printf("%d ", chro[i]);
    }
    printf("\n");
    free(chro);
    */

    pthread_join(thread_rand, NULL);
    pthread_join(thread_sahc, NULL);
    pthread_join(thread_rmhc, NULL);
    pthread_join(thread_ga,   NULL);

    /*
    int* path = malloc(10 * sizeof(int));
    CHECK_MALLOC_ERR(path); 
    for (int i=0; i<10; i++) {
        path[i] = i;
    }
    mutate_swap(&path, 10, 2, 6);
    for (int i=0; i<10; i++ ){
        printf("%d ", path[i]);
    }
    printf("\n");
    mutate_swap(&path, 10, 6, 2);
    for (int i=0; i<10; i++ ){
        printf("%d ", path[i]);
    }
    printf("\n");
    free(path);
    */

    fclose(fp);
    free(point_arr);
    free(point_arr_rand);
    free(point_arr_sahc);
    free(point_arr_rmhc);
    return 0;
}

