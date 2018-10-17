////////////////////////////////////////////////////////////////////////////////
// Main File:        (name of file with main function)
// This File:        (name of this file)
// Other Files:      (name of all other files if any)
// Semester:         CS 354 Spring 2018
//
// Author:           (griff zhang)
// Email:            (xzhang953@wisc.edu)
// CS Login:         (griff)
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
    int size;
    int **array;
} Square;

Square * construct_square(char *filename);
int verify_hetero(Square * square);
void insertion_sort(int* arr, int size);

int main(int argc, char *argv[])                        
{
    // Check input arguments to get filename
		
	if(argc!=2){
		printf("Usage: verify_hetero <filename>");
		return 0;
	}
	FILE* ptr = fopen(argv[1], "r");
	if(ptr==NULL){
		printf("cannot open file");
		return 0;
	}
	fclose(ptr);
    // Construct square
	Square* square = construct_square(argv[1]);
	if(square==NULL) {
		printf("maolloc fails");
		return 0;
	}	

    // Verify if it's a heterosquare and print true or false
	if(verify_hetero(square)==1) printf("true");
	else printf("false");
    return 0;
}

/* construct_square reads the input file to initialize a square struct
 * from the contents of the file and returns the square.
 * The format of the file is defined in the assignment specifications
 */
Square * construct_square(char *filename)                
{
	
    // Open and read the file
	int i, j;	
	char ary[60], temp;
	FILE* fr = fopen(filename, "r");	
    // Read the first line to get the square size
	fgets(ary, 60, fr);
    // Initialize a new Square struct of that size
	Square* new_square = malloc(sizeof(Square));
	if(new_square==NULL) return NULL;
	new_square -> size = atoi(ary);
	new_square -> array = malloc(sizeof(int*)*new_square->size);
	if(new_square->array==NULL) return NULL;
	*(new_square -> array) = malloc(sizeof(int*) * new_square->size * new_square->size);
	if(*(new_square->array)==NULL) return NULL;
	for(i=0; i<new_square->size; i++){
		*(new_square->array+i) = *(new_square->array)+(new_square->size)*i;
	}
    // Read the rest of the file to fill up the square
	for(i = 0; i<new_square->size; i++){
		for(j=0; j<new_square->size; j++){
			fscanf(fr, "%d%c", *(new_square->array+i)+j, &temp);
		}
	}
    return new_square;
}

/* verify_hetero verifies if the square is a heterosquare
 * 
 * returns 1(true) or 0(false)
 */
int verify_hetero(Square * square)               
{
    // Calculate sum of the following and store it in an array
    // all rows and cols
	//calculating the all the rows
	int dimension = square->size, i, j;
	int sum[dimension*2+2];
	for(i=0; i<dimension*2+2; i++){
		*(sum+i) = 0;
	}
	for(i=0; i<dimension; i++){
		for(j=0; j<dimension; j++){
			*(sum+i) += *(*(square->array+i)+j);
		}
	}
	int index = dimension;
	
	//calculatin columns
	for(i = 0; i<dimension; i++){
		for(j=0; j<dimension; j++){
			*(sum+index) += *(*(square->array+j)+i);//not sure
		}
		index++;
	}
	
    // main diagonal
	for(i=0; i<dimension; i++){
		*(sum+index) = *(*(square->array+i)+i);
	}	
	index++;
    // secondary diagonal
	for(i=0, j=dimension-1; i<dimension; i++, j--){
		*(sum+index) = *(*(square->array+i)+j);
	}	
	
    // Pass the array to insertion_sort function
	insertion_sort(sum, dimension*2+2);
    // Check the sorted array for duplicates
	for(i=0; i<dimension*2+1;i++){
		if(*(sum+i) ==*(sum+i+1)) return 0;
	}	

    return 1;
}

/* insertion_sort sorts the arr in ascending order
 *
 */
void insertion_sort(int* arr, int size)                  
{
    // Sort the arr
	int temp, i, j;
	for(i =0; i<size;i++){
		for(j=i+1; j>0 && arr[j]<arr[j-1];j--){
			temp = arr[j];
			arr[j] = arr[j-1];
			arr[j-1] = temp; 
		}
	}
}
