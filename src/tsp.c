/*
 * tsp.c
 * 
 * Find solution to traveling salesperson problem using genetic algorithms
 *
 */

#include <stdlib.h>
#include <string.h>  // strtok
#include <strings.h> // strcasecmp

#include "tsp.h"
#include "tsp-random.h"

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
    if (!check_malloc_err(point_arr)) exit(1);
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

    printf("Random Search:\n");    
    random_search(&point_arr, num_points, LT_GT);

    fclose(fp);
    free(point_arr);
    return 0;
}

