#ifndef LU_MISC_H_
#define LU_MISC_H

#include <stdio.h>
#include <stdlib.h>
#include "misc.h"

#define TAG 9999
#define MASTER 0

void read_transpose(double *A, double *l, double *u, double *b, int N, FILE *input) {
    int i, j;
    for (i=0; i<N; i++) {
        for (j=0; j<N+1; j++) {
            if (j<N) {
                fscanf(input, "%lf", &A[j *N+ i]);
                u[j *N+ i] = A[j *N+ i];
                if (i == j)
                    l[j *N+ i] = 1;
                else 
                    l[j *N+ i] = 0;
            } else {
                fscanf(input, "%lf", &b[i]);
            }
        }
    }
}

void calculateY(double L[], double Y[], double b[], int N) {
    int i, j;
    for (i = 0; i < N; i++) {
        Y[i] = b[i];
        for (j = i-1; j >= 0; j--) {
            Y[i] -= L[j*N + i] * Y[j];
        }
        Y[i] /= L[i*N + i];
    }
}

void calculateX(double U[], double X[], double Y[], int N) {
    int i, j;
    for (i = N-1; i > -1; i--) {
        X[i] = Y[i];
        for (j = i+1; j < N; j++) {
            X[i] -= U[j*N + i] * X[j];
        }
        X[i] /= U[i*N + i];
    }
}

int check_result(double *A,double *result, double *l, double *u, double *b, int N) {
    int i, j;
    double sum = 0.0, *y = (double *)malloc(N*sizeof(double));

    calculateY(l, y, b, N);
    calculateX(u, result, y, N);

    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {
            sum += A[j *N+ i]*result[j];
        }
        if (!dcmp(sum, b[i], 0.001)) {
            return false;
        }
        sum = 0.0;
    }
    return true;
}

#endif
