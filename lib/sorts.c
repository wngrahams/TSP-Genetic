/*
 * sorts.c
 *
 * Different kinds of sorts (just mergesort for now though)
 */

#include "sorts.h"

#define INSERTION_THRESHOLD 7

static inline int8_t _check_malloc_err(const void*);
void _merge(void***, const int, const int, const int, 
			unsigned long int (*eval) (void*));

/*
 * Merge Sort. Sorts the array passed in. l and r are the bounds of the 
 * sub array to be sorted, inclusive.
 */
void* merge_sort(void*** array, 
                 const int l, const int r,
                 unsigned long int (*eval) (void*)) {
    void **m;
    int mid;
   
    m = *array;

	if (l < r) {
		/*if ((r - l + 1) <= INSERTION_THRESHOLD) {
			insertion_sort(&(m+(sizeof(void*)*l)), (r-l+1), eval);
		}*/

		// avoid overflow
		mid = l+(r-l)/2;

		// recursively sort
		merge_sort(array, l, mid, eval);
		merge_sort(array, mid+1, r, eval);

		// merge
		_merge(array, l, mid, r, eval);
	}

	return 0;
}

/*
 * helper function for merge_sort
 */
void _merge(void*** array, 
		   const int l, const int m, const int r,
		   unsigned long int (*eval) (void*)) { 
	int i, j, k;
	int n1, n2;
	void** arr = *array;
	void **left, **right;

	n1 = m - l + 1; 
	n2 = r - m;

	// create temp arrays to hold the two pieces
	left = malloc(n1 * sizeof(void*));
	if (!_check_malloc_err(left)) exit(1);
	right = malloc(n2 * sizeof(void*));
	if (!_check_malloc_err(right)) exit(1);

	// copy data into temp arrays
	for (i=0; i<n1; i++) {
		left[i] = arr[l + i];
	}
	for (j=0; j<n2; j++) {
		right[i] = arr[m + 1+ j];
	}

	// merge temp arrays back into main arrays
	i = 0;
	j = 0;
	k = 0;
	while (i<n1 && j<n2) { 
        if (eval(left[i]) <= eval(right[j])) { 
            arr[k] = left[i]; 
            i++; 
        } 
        else { 
            arr[k] = right[j]; 
            j++; 
        } 
        k++; 
    }

	// put in any leftovers
	while (i < n1) { 
        arr[k] = left[i]; 
        i++; 
        k++; 
    } 
	while (j < n2) { 
        arr[k] = right[j]; 
        j++; 
        k++; 
    } 

	// free temp arrays
	free(left);
	free(right);
}

/*
 * Insertion sort
 */
void* insertion_sort(void*** a, const int len, 
					 unsigned long int (*eval) (void*)) {
	
	void** arr;
	int i, j;
    unsigned long int key;
    void* temp; 
    
	arr = *a;

	for (i = 1; i < len; i++) { 
        key = eval(arr[i]); 
        temp = arr[i];
        j = i - 1; 
  
        /* Move elements of arr[0..i-1], that are 
          greater than key, to one position ahead 
          of their current position */
        while (j >= 0 && eval(arr[j]) > key) { 
            arr[j + 1] = arr[j]; 
            j = j - 1; 
        } 
        arr[j + 1] = temp; 
    } 

	return 0;
}

static inline int8_t _check_malloc_err(const void *ptr) {
    if (NULL == ptr) {
        perror("malloc() returned NULL");
        return 0;         
    } /* END if */

    return 1;
}
