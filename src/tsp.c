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
                                &point_arr[path[MOD(i+1, num_points)]]);
    }

    printf("Initial total distance: %.9lf\n", total_dist);

    // open file for writing
    FILE *f_results = fopen("./output/out-random.txt", "a");
    if (NULL == f_results) {
        perror("./output/out-random.txt");
        exit(1);
    }

    fprintf(f_results, "0\t%lf\n", total_dist);
    
    unsigned long int num_evaluations = 1L;
    unsigned long int last_change = 1L;
    double new_dist = total_dist;

    while (num_evaluations < MIN_ITER || num_evaluations/1.25 <= last_change) {
        
		// repeatedly swap two, find new distance, keep them if it's better

	    //choose two to swap
    	int pos1 = rand()%num_points;
    	int pos2;
    	do {
        	pos2 = rand()%num_points;
    	} while (pos2 == pos1);

    	// substract distance caused by the original placement 
		// of these two points
    	new_dist = total_dist;
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
    	
        //printf("New distance: %.9lf\n", new_dist);
    	
        // if new_dist is less than old_dist, keep the swapped points
    	if (lt_gt(new_dist, total_dist, LT_GT)) {
            total_dist = new_dist;
            last_change = num_evaluations;
            // write to file
            fprintf(f_results, "%lu\t%lf\n", num_evaluations, total_dist);
    	}
    	else {
        	temp = path[pos2];
        	path[pos2] = path[pos1];
        	path[pos1] = temp;
    	}
		
        num_evaluations++;
    }

    fprintf(f_results, "%lu\t%lf\n", num_evaluations, total_dist);
    printf("Final distance: %.9lf\n", total_dist);

    // Write final path to file
    FILE *f_path = fopen("./output/out-random-path.txt", "a");
    if (NULL == f_path) {
        perror("./output/out-random-path.txt");
        exit(1);
    }

    fprintf(f_path, "%lf\n", total_dist);
    for (int i=0; i<num_points; i++) {
        fprintf(f_path, "%d\n", path[i]);
    }
//    fprintf(f_path, "\n");

    fclose(f_path);
    fclose(fp);
    fclose(f_results);
    free(point_arr);
    free(path);
    return 0;
}

