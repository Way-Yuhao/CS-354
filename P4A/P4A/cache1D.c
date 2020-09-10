////////////////////////////////////////////////////////////////////////////////
// Main File:
// This File:        cache1D.c
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

int arr[100000];
int main(int argc, char* argv[]) {
    for (int i = 0; i < 100000; i++) {
        arr[i] = i;
    }
    return 0;
}

