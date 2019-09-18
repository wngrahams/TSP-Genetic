/*
 * tsp.c
 * 
 * Find solution to traveling salesperson problem using genetic algorithms
 *
 */

#include <stdio.h>

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

    fclose(fp);
    return 0;
}
