/*
 * tsp.c
 * 
 * Find solution to traveling salesperson problem using genetic algorithms
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>  // int8_t
#include <assert.h>  // assert
#include <string.h>  // strtok

struct point {
    double x;
    double y;
};

int8_t check_malloc_err(const void*);

int main(int argc, char** argv) {

    // open tsp datapoints file specified in command line
    if (argc != 2) {
        fprintf(stderr, "%s\n", "usage: tsp <tsp_file>");
        exit(1);
    }

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

    for (int i=0; i<num_points; i++) {
        printf("x = %.9lf, y = %.9lf\n", point_arr[i].x, point_arr[i].y);
    }

    fclose(fp);
    free(point_arr);
    return 0;
}

/* This function checks if malloc() returned NULL. If it did, the program
 * prints an error message. The function returns 1 on success and 0 on failure
 */ 
int8_t check_malloc_err(const void *ptr) {
	if (NULL == ptr) {
		perror("malloc() returned NULL");
		return 0;         
	} /* END if */

	return 1;
}

