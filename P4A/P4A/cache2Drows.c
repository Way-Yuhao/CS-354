////////////////////////////////////////////////////////////////////////////////
// Main File:
// This File:        cache2Drows.c
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
// Online sources:   None
//
//////////////////////////// 80 columns wide ///////////////////////////////////
#include <stdio.h>
int arr2D[3000][500];
int main(int argc, char* argv[]){
    for (int row = 0; row < 3000; row++) {
        for (int col = 0; col < 500; col++) {
            arr2D[row][col] = row + col;
        }
    }
    return 0;
}
