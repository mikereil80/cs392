/*******************************************************************************
 * Name        : quicksort.c
 * Author      : Michael Reilly
 * Date        : 2/21/20
 * Description : Quicksort implementation.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "quicksort.h"

/* Static (private to this file) function prototypes. */
static void swap(void *a, void *b, size_t size);
static int lomuto(void *array, int left, int right, size_t elem_sz,
                  int (*comp) (const void*, const void*));
static void quicksort_helper(void *array, int left, int right, size_t elem_sz,
                             int (*comp) (const void*, const void*));

/**
 * Compares two integers passed in as void pointers and returns an integer
 * representing their ordering.
 * First casts the void pointers to int pointers.
 * Returns:
 * -- 0 if the integers are equal
 * -- a positive number if the first integer is greater
 * -- a negative if the second integer is greater
 */
int int_cmp(const void *a, const void *b) {
    // TODO
	int *i1=(int*)a;
	int *i2=(int*)b;
	if((*i1)>(*i2)){
		return 1;
	}
	else if ((*i1)<(*i2)){
		return -1;
	}
	else{
		return 0;
	}
}

/**
 * Compares two doubles passed in as void pointers and returns an integer
 * representing their ordering.
 * First casts the void pointers to double pointers.
 * Returns:
 * -- 0 if the doubles are equal
 * -- 1 if the first double is greater
 * -- -1 if the second double is greater
 */
int dbl_cmp(const void *a, const void *b) {
    // TODO
	double *d1=(double*)a;
	double *d2=(double*)b;
	if((*d1)>(*d2)){
		return 1;
	}
	else if ((*d1)<(*d2)){
		return -1;
	}
	else{
		return 0;
	}
}

/**
 * Compares two char arrays passed in as void pointers and returns an integer
 * representing their ordering.
 * First casts the void pointers to char* pointers (i.e. char**).
 * Returns the result of calling strcmp on them.
 */
int str_cmp(const void *a, const void *b) { 
    // TODO
	char *s1=*(char**)a;
	char *s2=*(char**)b;
	return strcmp(s1,s2);
}

/**
 * Swaps the values in two pointers.
 *
 * Casts the void pointers to character types and works with them as char
 * pointers for the remainder of the function.
 * Swaps one byte at a time, until all 'size' bytes have been swapped.
 * For example, if ints are passed in, size will be 4. Therefore, this function
 * swaps 4 bytes in a and b character pointers.
 */
static void swap(void *a, void *b, size_t size) {
    // TODO
	char *a1=a;
	char *b1=b;
	do{
		char temp=*a1;
		*a1++=*b1;
		*b1++=temp;
	}
	while(--size>0);
}

/**
 * Partitions array around a pivot, utilizing the swap function.
 * Each time the function runs, the pivot is placed into the correct index of
 * the array in sorted order. All elements less than the pivot should
 * be to its left, and all elements greater than or equal to the pivot should be
 * to its right.
 * The function pointer is dereferenced when it is used.
 * Indexing into void *array does not work. All work must be performed with
 * pointer arithmetic.
 */
static int lomuto(void *array, int left, int right, size_t elem_sz,
                  int (*comp) (const void*, const void*)) {
    // TODO
	char *carray=(char *)array;
	int partition,middle=(left+right)/2;
	partition=comp(&carray[left*elem_sz],&carray[middle*elem_sz])>=1 ? left : middle;
	if(comp(&carray[partition*elem_sz],&carray[right*elem_sz])>=1){
		partition=right;
	}
	swap(&carray[partition*elem_sz],&carray[left*elem_sz],elem_sz);
	partition=left;
	while(left<right){
		if(comp(&carray[left*elem_sz],&carray[right*elem_sz])<=0){
			swap(&carray[partition*elem_sz],&carray[left*elem_sz],elem_sz);
			partition++;
		}
		left++;
	}
	swap(&carray[partition*elem_sz],&carray[right*elem_sz],elem_sz);
	return partition;
}

/**
 * Sorts with lomuto partitioning, with recursive calls on each side of the
 * pivot.
 * This is the function that does the work, since it takes in both left and
 * right index values.
 */
static void quicksort_helper(void *array, int left, int right, size_t elem_sz,
                             int (*comp) (const void*, const void*)) {
    // TODO
	// Will cancel when left is greater than or equal to right, as then it should be sorted
	if(left>=right){
		return;
	}
	else{
		// calls the lomuto partition to find where the current pointer goes, and then moves things accordingly
		int partition=lomuto(array, left, right, elem_sz, comp);
		
		quicksort_helper(array,left,partition-1,elem_sz,comp);
		quicksort_helper(array,partition+1,right,elem_sz,comp);
	}
}

/**
 * Quicksort function exposed to the user.
 * Calls quicksort_helper with left = 0 and right = len - 1.
 */
void quicksort(void *array, size_t len, size_t elem_sz,
               int (*comp) (const void*, const void*)) {
	// TODO
	int right=(int)(len-1);
    quicksort_helper(array, 0, right, elem_sz, comp);
}