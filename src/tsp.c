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

struct point_as_string {
    char* x;
    char* y;
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
    char* x_str = "";
    char* y_str = "";
    char line[1024];
    char* separators = "\t \n";
    double x, y;

    while (fgets(line, sizeof(line), fp)) {
        x_str = strtok(line, separators);
        y_str = strtok(NULL, separators);

        x = atof(x_str);
        y = atof(y_str);

        num_points++;
    }

    printf("Number of points in the file: %d\n", num_points);

    fclose(fp);
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
