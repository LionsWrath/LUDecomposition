#ifndef MISC_H_
#define MISC_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define true 1
#define false 0

void print_vector(double X[], int N) {
    int i;

    for (i = 0; i < N; i++) {
        printf("%-5.2lf ", X[i]);
    }

    printf("\n\n");
}

void print_matrix(double X[], int N) {
    int i,j;

    printf("\n");
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%-5.2lf ", X[i *N+ j]);
        }
        printf("\n");
    }

    printf("\n\n");
}

void malloc_matrix(double **matrix, int N) {
    *matrix = (double *)malloc(N*N*sizeof(double));
}

void malloc_vector(double **vector, int N) {
    *vector = (double *)malloc(N*sizeof(double));
}

void malloc_all(double **A, double **l, double **u, double **b, int N) {
    malloc_matrix(A, N);
    malloc_matrix(u, N);
    malloc_matrix(l, N);

    malloc_vector(b, N);
}

void free_all(double *A, double *l, double *u, double *b) {
    free(A);
    free(l);
    free(u);
    free(b);
}


void open_file(FILE **file, char filename[]) {
    *file = fopen(filename, "r");
    if (file == NULL) {
        printf("ERRO: Arquivo incorreto.");
        exit(1);
    }
}

int dcmp(double x, double y, double precision) {
    if (x == y) return true;
    if (fabs(x-y) <= precision) {
        return true;
    }
    return false;
}

#endif
