////////////////////////////////////////////////////////////////////////////////
// Main File:        traverse_spiral.c
// This File:        traverse_spiral.c
// Other Files:      None
// Semester:         CS 354 Spring 2019
//
// Author:           Yuhao Liu
// Email:            liu697@wisc.edu
// CS Login:         yuhao
//         
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          None
//
// Online sources: None
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *COMMA = ","; 

// Structure representing Matrix
// size: Dimension of the square (size*size)
// array: 2D array of integers
typedef struct _Square { 
    int size;
    int **array;
} Square; 

/*
 * Retrieves from the first line of the input file,
 * the size of the square matrix.
 * 
 * fp: file pointer for input file
 * size: pointer to size of the square matrix
 */
void get_dimensions(FILE *fp, int *size) {
    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, fp) == -1) {
        fprintf(stderr, "Error in reading the file\n");
        fclose(fp);
        exit(1);
    }
    char *token = NULL;
    token = strtok(line, COMMA);
    *size = atoi(token);
}

/* 
 * Traverses a given layer from the square matrix
 * 
 * array: Heap allocated 2D square matrix
 * size: size of the 2D square matrix
 * layer: layer number to be traversed
 * op: pointer to the output file
 */
void traverse_layer(int **array, int size, int layer, FILE *op) {  
	int row_num, col_num;
	int i,j;
	
	//corner case: size is odd & the layer is last so only one entry to print
        if(size % 2 == 1 && layer == (size + 1) / 2 - 1){
            fprintf(op, "%d\n", *(*(array + layer) + layer));
            return;
        }

	//Traverse upper row from left to right with appropriate bounds
	row_num = layer;
    i = row_num;
    for(j = row_num; j < size - layer - 1; j++) {
        fprintf(op, "%d ", *(*(array + i) + j));
	}

	//Traverse right column from top to bottom with appropriate bounds
	col_num = size-layer-1;
    j = col_num;
    for(i = row_num; i < size-layer-1; i++) {
        fprintf(op, "%d ", *(*(array+i) + j));
	}

	//Traverse lower row from right to left with appropriate bounds
	row_num = size-layer-1;
    i = col_num;
    for(j = size-layer-1; j > layer; j--) {
         fprintf(op, "%d ", *(*(array+i) + j));
	}

	//Traverse left column from bottom to top with appropriate bounds
	col_num = layer;
    j = col_num;
    for(i = size-layer-1; i > layer; i--) {
        fprintf(op, "%d ", *(*(array+i) + j));
	}
}


/*
 * Traverses the square matrix spirally
 * 
 * square: pointer to a structure that contains 2D square matrix
 * op: pointer to the output file
 */
void traverse_spirally(Square *square, FILE *op) {         
	int size = square->size;
    
	int num_layers = 0;   
	num_layers = size/2; 
	if(size%2 == 1) {
		num_layers++;
	}
	 
	int i;
	for(i = 0; i < num_layers; i++) {
		traverse_layer(square->array, size, i, op);
	}
}

/*
 * This program reads a square matrix from the input file
 * and outputs its spiral order traversal to the output file
 *
 * argc: CLA count
 * argv: CLA value
 */
  
int main(int argc, char *argv[]) {
    int size = 0;
    int *pts = &size;
    
    //Check if number of command-line arguments is correct
    if (argc != 3) {
        fprintf(stderr, "Usage: ./traverse_spiral <input_filename> <output_filename>");
        exit(2);
    }
    
	//Open the file and check if it opened successfully	
    FILE *fp = fopen(*(argv + 1), "r");
    if (fp == NULL) {
        printf("Cannot open file for reading\n");
        fclose(fp);
        exit(1);
    }
    
	//Call the function get_dimensions to retrieve size of the square matrix
    get_dimensions(fp, pts);
    
	//Dynamically allocate a 2D array as per the retrieved dimensions
    int **board = malloc(sizeof(int*) * size);
    for (int i = 0; i < size; i++) {
        *(board + i) = malloc(sizeof(int) * size);
    }

	//Read the file line by line by using the function getline as used in get_dimensions
	//Tokenize each line wrt comma to store the values in the square matrix
    char *line = NULL;
    size_t len = 0;
    char *token = NULL;
    for (int i = 0; i < size; i++) {
        if (getline(&line, &len, fp) == -1) {
            printf("Error while reading the file\n");
            fclose(fp);
            exit(1);
        }
        token = strtok(line, COMMA);
        for (int j = 0; j < size; j++) {
            *(*(board + i) + j) = atoi(token);
            token = strtok(NULL, COMMA);
        }
    }
    
	//Create a structure and initialize its size and array fields appropriately
    Square *sq = malloc(sizeof(Square));
    sq -> array = board;
    sq -> size = size;

	//Open the output file
    FILE *ofp = fopen(*(argv + 2), "w");
    if (ofp == NULL) {
        printf("Cannot open file for output\n");
        fclose(fp);
        fclose(ofp);
        exit(1);
    }

	//Call the function traverse_spirally
    traverse_spirally(sq, ofp);

	//Free the dynamically allocated memory
    for (int i = 0; i < size; i++) {
        free(*(board + i));
    }
    free(board);
    free(sq);
    
	//Close the input file
    if (fclose(fp) != 0) {
        printf("Error while closing the input file\n");
        exit(1);
    }
	//Close the output file
    if (fclose(ofp) != 0) {
        printf("Error while closing the output file\n");
        exit(1);
    }
    return 0;
}         
