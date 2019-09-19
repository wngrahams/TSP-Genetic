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
#include <time.h>    // rand

// Citation: modulo macro from https://www.lemoda.net/c/modulo-operator/
// for correct output when taking the mod of a negaitve number
#define MOD(a,b) ((((a)%(b))+(b))%(b))

struct point {
    double x;
    double y;
};

int8_t check_malloc_err(const void*);

double calc_dist(const struct point*, const struct point*);

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

    // array for path
    int* path = malloc(num_points * sizeof(int));

    // randomly assign path, point 0 is the start
    srand((unsigned int)time(NULL));

    // first fill array sequentially, then shuffle
    for (int i=0; i<num_points; i++) {
        path[i] = i;
    }

    // citation: shuffle algorithm by Ben Pfaff
    // https://benpfaff.org/writings/clc/shuffle.html
    srand((unsigned int)time(NULL));
    int switch_pos, temp;
    for (int i=0; i<num_points; i++) {
        switch_pos = i + rand() / (RAND_MAX/(num_points - i) + 1);
        temp = path[switch_pos];
        path[switch_pos] = path[i];
        path[i] = temp;
    }
    
    double total_dist = 0.0;
    // calculate initial total distance
    for (int i=0; i<num_points; i++) {
        total_dist += calc_dist(&point_arr[path[i]], 
                                &point_arr[MOD(i+1, num_points)]);
    }

    printf("Initial total distance: %.9lf\n", total_dist);
    
    counter = 0;
    while (counter++ < 1000) {
        
		// repeatedly swap two, find new distance, keep them if it's better

	    //choose two to swap
    	int pos1 = 0; //rand()%num_points;
    	int pos2;
    	do {
        	pos2 = rand()%num_points;
    	} while (pos2 == pos1);

    	// substract distance caused by the original placement 
		// of these two points
    	double new_dist = total_dist;
    	new_dist -= ( calc_dist(&point_arr[path[MOD(pos1-1, num_points)]],
        	                    &point_arr[path[pos1]]) + 
            	      calc_dist(&point_arr[path[pos1]],
                	            &point_arr[path[MOD(pos1+1, num_points)]]) + 
                  	  calc_dist(&point_arr[path[MOD(pos2-1, num_points)]],
                      	        &point_arr[path[pos2]]) +
                  	  calc_dist(&point_arr[path[pos2]],
                      	        &point_arr[path[MOD(pos2+1, num_points)]]) );

    	// swap the points
    	temp = path[pos2];
    	path[pos2] = path[pos1];
    	path[pos1] = temp;

    	// add distance caused by the new placement
    	new_dist += ( calc_dist(&point_arr[path[MOD(pos1-1, num_points)]],
        	                    &point_arr[path[pos1]]) +
            	      calc_dist(&point_arr[path[pos1]],  
                	            &point_arr[path[MOD(pos1+1, num_points)]]) +
                  	  calc_dist(&point_arr[path[MOD(pos2-1, num_points)]],
                      	        &point_arr[path[pos2]]) +
                  	  calc_dist(&point_arr[path[pos2]],
                      	        &point_arr[path[MOD(pos2+1, num_points)]]) );

    	printf("New total distance: %.9lf\n", new_dist);

    	// if new_dist is less than old_dist, keep the swapped points
    	if (new_dist < total_dist) {
        	total_dist = new_dist;
    	}
    	else {
        	temp = path[pos2];
        	path[pos2] = path[pos1];
        	path[pos1] = temp;
    	}
		
    }

    fclose(fp);
    free(point_arr);
    free(path);
    return 0;
}

/* 
 * This function checks if malloc() returned NULL. If it did, the program
 * prints an error message. The function returns 1 on success and 0 on failure
 */ 
int8_t check_malloc_err(const void *ptr) {
	if (NULL == ptr) {
		perror("malloc() returned NULL");
		return 0;         
	} /* END if */

	return 1;
}

/*
 * Calculates the euclidean distance between two points
 */
double calc_dist(const struct point* p1, const struct point* p2) {
    return hypot(p2->x - p1->x, p2->y - p1->y);
}

