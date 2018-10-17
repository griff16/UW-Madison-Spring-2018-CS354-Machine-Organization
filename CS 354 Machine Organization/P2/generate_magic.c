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

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
    int size;
    int **array;
} Square;

int get_square_size();
Square * generate_magic(int size);
void write_to_file(char *filename, Square * square);

int main(int argc, char *argv[])                 
{
    // Check input arguments to get filename
	FILE* fr = fopen(*(argv+1),"w");
	if(argc != 2 || fr==NULL) {
		printf("cannot open the file");
		return 0;
	}
	fclose(fr);
	
    // Get size from user
	int input = get_square_size();
	    // Generate the magic square
	Square* square = generate_magic(input);
	if(square==NULL){
		printf("malloc fails");
		return 0;
	}
    // Write the square to the output file
	write_to_file(*(argv+1), square);
	free(*(square->array));
	free(square->array);
    return 0;
}

/* get_square_size prompts the user for the magic square size
 * checks if it is an odd number >= 3 and returns the number
 */
int get_square_size()            
{	
	printf("Enter size of magic square, must be odd\n");
	int input;
	scanf("%d", &input); 
	if(input%2!=1 || input<3) {
		printf("Size must be an odd number >= 3.");
		return 0;
	}
	return input;
}

/* generate_magic constructs a magic square of size n
 * using the Siamese algorithm and returns the Square struct
 */
Square * generate_magic(int n)           
{
	int i, x, y;
	Square* square = malloc(sizeof(Square));
	if(square==NULL) return NULL;
	square -> size = n;
	square -> array = malloc(sizeof(int*)*square->size);
	if(square->array==NULL) return NULL;
	*(square -> array) = malloc(sizeof(int)*square->size*square->size);
	if(*(square->array)==NULL) return NULL;
	for(i=0; i<square->size;i++){
		*(square->array+i) = *(square->array)+square->size*i;
	}

	x= 0; y=n/2;
	for(i=1; i<=n*n;i++){
		*(*(square->array+x)+y) = i;
		if(i%n==0){//if i is multiple of n then we the coordinates need to go down
			x = (x+1)%n;
		}	
		else {// it controls the upper right movement
			x = (x-1+n)%n;
			y = (y+1)%n;
		}
	}
	printf("1");

	return square;
}

/* write_to_file opens up a new file(or overwrites the existing file)
 * and writes out the square in the format expected by verify_hetero.c
 */
void write_to_file(char *filename, Square * square)              
{
    // Write the square to file
	FILE* fr = fopen(filename, "w");
	int i;
	fprintf(fr,"%d\n", square->size);
	for(i=0; i<square->size*square->size;i++){
		fprintf(fr, "%d", *(*(square->array)+i));
		if((i+1)%square->size==0) fprintf(fr, "\n");
		else fprintf(fr,",");
	}
	fclose(fr);
}
