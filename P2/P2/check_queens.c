////////////////////////////////////////////////////////////////////////////////
// Main File:        check_queens.c
// This File:        check_queens.c
// Other Files:
// Semester:         CS 354 SPRING 2019
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
// Online sources:   None
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h> 
#include <stdlib.h>  
#include <string.h>   
 
char *COMMA = ",";
  
/* COMPLETED:
 * Retrieves from the first line of the input file,
 * the number of rows and columns for the board.
 * 
 * fp: file pointer for input file
 * rows: pointer to number of rows
 * cols: pointer to number of columns
 */
void get_dimensions(FILE *fp, int *rows, int *cols) {  
        char *line = NULL;  
        size_t len = 0;  
        if (getline(&line, &len, fp) == -1) {  
                printf("Error in reading the file\n");  
                exit(1);  
        }
        char *token = NULL; 
        token = strtok(line, COMMA);
        *rows = atoi(token); 
           
        token = strtok(NULL, COMMA); 
        *cols = atoi(token);
}      


/*
 * Returns 1 if and only if there exists at least one pair
 * of queens that can attack each other.
 * Otherwise returns 0.
 * 
 * board: heap allocated 2D board
 * rows: number of rows
 * cols: number of columns
 */
int check_queens(int **board, int rows, int cols) {  
    int local_sum = 0; //stores the number of queens on each line per iteration
    //check horizontal pairs
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++){
            //check if queen exists in a certian place
            if (*(*(board + i) + j) == 1) {
                local_sum += 1;
                if (local_sum >= 2) {
                    //return 1 when there is a conflict
                    return 1;
                }
            }
        }
        local_sum = 0; //reset counter
    }
 
    //check vertical pairs
    local_sum = 0;
    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            //check if queen exists in a certian place
            if (*(*(board + i) + j) == 1) {
                local_sum += 1;
                if (local_sum >= 2){
                    //return 1 when there is a conflict
                    return 1;
                }
            }
        }
        local_sum = 0; //FIXME:
    }
    
    //check diagonal pairs
    local_sum = 0;
    //projects the number of queens in each diagnal line to a 1-D array
    int *left_diag_sums = malloc(sizeof(int) * (rows + cols - 1));
    int *right_diag_sums = malloc(sizeof(int) * (rows + cols - 1));
    
    //initialize the two 1-D arrays
    for (int i=0; i<rows; i++){
        left_diag_sums[i]=0;
        right_diag_sums[i]=0;
    }

    //iterate through the entire board
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (*(*(board + i) + j) == 1) {
                //at each cell, check if there is conflict at the two intersecting diagnal lines
                left_diag_sums[i+j] += 1;
                right_diag_sums[j-i+rows-1] += 1;
                //if there is a conflict
                if (left_diag_sums[i+j] > 1 || right_diag_sums[j-i+rows-1]>1) {
                    //free up memory space before returning
                    free(left_diag_sums);
                    free(right_diag_sums);
                    return 1;
            }
        }
    }
 }
    //if there is no conflicts, free up memory space and return 0
    free(left_diag_sums);
    free(right_diag_sums);
    return 0;
}     


/*
 * This program prints true if the input file has any pair
 * of queens that can attack each other, and false otherwise
 * 
 * argc: CLA count
 * argv: CLA value 
 */
int main(int argc, char *argv[]) {
    //Check if number of command-line arguments is correct.
    if (argc != 2) {
        fprintf(stderr, "Usage: ./check_queens <input_filename>");
        exit(2);
    }

    //Open the file and check if it opened successfully.
    FILE *fp = fopen(*(argv + 1), "r");
    if (fp == NULL) {
            printf("Cannot open file for reading\n");
            exit(1);
    }

    //Declare local variables.
    int rows = 0;
    int cols = 0;
    int *pt_rows = &rows;
    int *pt_cols = &cols;
    //Call get_dimensions to retrieve the board dimensions.
    get_dimensions(fp, pt_rows, pt_cols);

    //Dynamically allocate a 2D array of dimensions retrieved above.
    int **board = malloc(sizeof(int*) * rows);
    for (int i = 0; i < rows; i++) {
        *(board+i) = malloc(sizeof(int) * cols);
    }

    //Read the file line by line.
    //Tokenize each line wrt comma to store the values in your 2D array.
    char *line = NULL;
    size_t len = 0;
    char *token = NULL;
    for (int i = 0; i < rows; i++) {
        if (getline(&line, &len, fp) == -1) {
            printf("Error while reading the file\n");
            exit(1);
        }

        token = strtok(line, COMMA);
        for (int j = 0; j < cols; j++) {
            //Complete the line of code below
            //to initialize your 2D array.
            *(*(board + i) + j)= atoi(token);
            token = strtok(NULL, COMMA);
        }
    }

    //Call the function check_queens and print the appropriate
    //output depending on the function's return value.
    int result = check_queens(board, rows, cols);
    if (result == 1){
        printf("true");
    } else {
        printf("false");
	}

    //Free all dynamically allocated memory.
    for (int i = 0; i < rows; i++) {
        free(*(board + i));
    }
    free(board);

    //Close the file.
    if (fclose(fp) != 0) {
            printf("Error while closing the file\n");
            exit(1);
    }
    return 0;
}      
