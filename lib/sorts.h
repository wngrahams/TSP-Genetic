/*
 * sorts.h
 */

#ifndef _SORTS_H_
#define _SORTS_H_

#include <stdlib.h>
#include <stdio.h>

void* merge_sort(void***, 
                 const int, const int, 
                 unsigned long int (*eval) (void*));

void* insertion_sort(void***,
                     const int,
                     unsigned long int (*eval) (void*));

#endif /* _SORTS_H_ */
