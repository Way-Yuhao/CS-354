//
//  main.c
//  Tests
//
//  Created by Liu Yuhao on 2/11/19.
//  Copyright © 2019 Apple. All rights reserved.
//

#include <stdio.h>



int main(int argc, const char * argv[]) {
    int *a = {1, 2, 3};
    a[17] = 22;
    printf("%d", a[7]);
}

