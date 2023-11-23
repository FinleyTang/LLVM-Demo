//
// Created by ftang on 2023/11/23.
//
#include <stdio.h>
#include <stdlib.h>

void testMalloc() {
    int* ptr = (int*)malloc(sizeof(int));
    if (ptr != NULL) {
        *ptr = 10;
        printf("Malloc: %d\n", *ptr);
        free(ptr);
    }
}

void testCalloc() {
    int* ptr = (int*)calloc(5, sizeof(int));
    if (ptr != NULL) {
        for (int i = 0; i < 5; i++) {
            ptr[i] = i;
        }
        printf("Calloc: ");
        for (int i = 0; i < 5; i++) {
            printf("%d ", ptr[i]);
        }
        printf("\n");
        free(ptr);
    }
}

void testRealloc() {
    int* ptr = (int*)malloc(5 * sizeof(int));
    if (ptr != NULL) {
        printf("Realloc (Before): ");
        for (int i = 0; i < 5; i++) {
            ptr[i] = i;
            printf("%d ", ptr[i]);
        }
        printf("\n");

        ptr = (int*)realloc(ptr, 10 * sizeof(int));
        if (ptr != NULL) {
            printf("Realloc (After): ");
            for (int i = 0; i < 10; i++) {
                ptr[i] = i;
                printf("%d ", ptr[i]);
            }
            printf("\n");
            free(ptr);
        }
    }
}
