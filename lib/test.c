/*
 * test.c for sorting stuff
 */

#include "sorts.h"

struct indiv {
	int idx;
	double len;
};

/* Function to print an array */
void printArray(struct indiv** A, int size) 
{ 
    int i; 
    for (i=0; i < size; i++) 
        printf("%f ", (A[i])->len); 
    printf("\n"); 
} 

unsigned long int fitness(void* amt) {
	struct indiv* i = (struct indiv*)amt;
	return (unsigned long int)(i->len * 1000);
}
  
/* Driver program to test*/
int main() 
{ 
    struct indiv** arr = malloc(6 * sizeof(struct indiv*)); 
	struct indiv a0 = {
		.idx = 0,
		.len = 0.012
	};
	struct indiv a1 = {
		.idx = 1,
		.len = 6.67
	};
	struct indiv a2 = {
		.idx = 2,
		.len = 12.33
	};
	struct indiv a3 = {
		.idx = 3,
		.len = 1.34
	};
	struct indiv a4 = {
		.idx = 4,
		.len = 8.2
	};
	struct indiv a5 = {
		.idx = 5,
		.len = 24.51
	};
	arr[0] = &a0;
	arr[1] = &a1;    
	arr[2] = &a2;    
	arr[3] = &a3;    
	arr[4] = &a4;    
	arr[5] = &a5;    
    int arr_size = sizeof(arr)/sizeof(arr[0]); 
  
    printf("Given array is \n"); 
    printArray(arr, arr_size);

    struct indiv*** arrptr = &arr;
  
    merge_sort((void***)arrptr, 0, arr_size - 1, fitness); 
  
    printf("\nSorted array is \n"); 
    printArray(arr, arr_size); 
    free(arr);
	return 0; 
} 
