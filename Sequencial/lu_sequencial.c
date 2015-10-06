#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define true 1
#define false 0

void calculateY(double L[], double Y[], double b[], int N) {
    for (int i = 0; i < N; i++) {
        Y[i] = b[i];
        for (int j = 0; j < i; j++) {
            Y[i] -= L[i*N + j] * Y[j];
        }
        Y[i] /= L[i*N + i];
    }
}

void calculateX(double U[], double X[], double Y[], int N) {
    for (int i = N-1; i > -1; i--) {
        X[i] = Y[i];
        for (int j = N-1; j > i; j--) {
            X[i] -= U[i*N + j] * X[j];
        }
        X[i] /= U[i*N + i];
    }
}

int dcmp(double x, double y, double precision) {
    if (x == y) return true;
    if (fabs(x-y) <= precision) {
        return true;
    }
    return false;
}

int check_result(double *A, double *X, double *Y, double *L, double *U, double *b, int N) {
    int i, j;
    double sum = 0.0;

    calculateY(L, Y, b, N);
    calculateX(U, X, Y, N);

    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {
            sum += A[i *N+ j]*X[j];
        }
        if (!dcmp(sum, b[i], 0.001)) {
            return false;
        }
        sum = 0.0;
    }
    return true;
}

void crout(double A[], double L[], double U[], int N) {
    int i, j, k;

    for (i = 0; i < N; i++) {
        L[i*N + i] = 1;
    }

    for (j = 0; j < N; j++) {
        for (i=0; i<=j; i++) {
            U[i*N + j] = A[i*N + j];
            for (k=0; k<i; k++) {
                U[i*N + j] -= L[i*N + k] * U[k*N + j];
            }
        }

        for (i = j; i < N; i++) {
            L[i*N + j] = A[i*N + j];
            for (k=0; k<j; k++) {
                L[i*N + j] -= L[i*N + k] * U[k*N + j];
            }
            L[i*N + j] *= 1.00 / U[j*N + j];
        }
    }
}

void print_vector(double X[], int N) {
    int i;
   
    for (i = 0; i < N; i++) {
        printf("%-5.2lf ", X[i]);
    }

    printf("\n\n"); 
}

double main(int argc, char *argv[]) {
    int i, j, oc, N;
    double *A, *L, *U, *b, *X, *Y;
    FILE *in, *out = stdout;
    char filename[50];
    clock_t stime, ftime;
    
    while ((oc = getopt(argc, argv, "s:i:")) != -1) {
        switch (oc) {
            case 's':
                N = atoi(optarg);
                break;
            case 'i':
                strcpy(filename, optarg);
                break;
            case ':':
                printf("1");
                break;
            case '?':
            default:
                printf("2");
                break;
        }
    }

    in = fopen(filename, "r");
    if (in == NULL) {
        printf("ERRO: Arquivo incorreto. %s %d\n", filename, N);
        exit(1);
    }

    A = (double *)malloc(N*N*sizeof(double));
    L = (double *)malloc(N*N*sizeof(double));
    U = (double *)malloc(N*N*sizeof(double));

    b = (double *)malloc(N*sizeof(double));
    X = (double *)malloc(N*sizeof(double));
    Y = (double *)malloc(N*sizeof(double));


    for (i=0;i<N;i++) {
        for (j=0;j<N+1;j++) {
            if (j<N) {
                fscanf(in, "%lf", &A[i*N + j]);
            } else {
                fscanf(in, "%lf", &b[i]);
            }
        }
    }

    stime = clock();

    crout(A,L,U,N);

    ftime = clock();

    if (check_result(A, X, Y, L, U, b, N)) {
        fprintf(out, "( %s ): %lf\n", filename, (double)(ftime-stime)/CLOCKS_PER_SEC);
    } else {
        fprintf(out, "( %s ): ERRO NA VERIFICAÇÃO\n", filename);
    }

    //print_vector(X, N);

    free(A);
    free(L);
    free(U);
    free(b);
    free(X);
    free(Y);

    return 0;
}
