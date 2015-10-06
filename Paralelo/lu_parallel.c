#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "misc.h"
#include "lu_misc.h"

void decomposeMPI(int n ,int nprocs , int pid) {
    int i, j, k, aux;
    double *l = (double *)malloc(n*sizeof(double));
    double *u = (double *)malloc(n*n*sizeof(double));

	//Receber coluna da matriz U
    for (i=pid-1; i<n; i+=(nprocs-1)) {
        MPI_Recv(&u[i*n], n, MPI_DOUBLE, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (k=0; k<n-1; k++) {
        if (k%(nprocs-1)+1 == pid) {
            for (j=k+1; j<n; j++) {
                l[j] = u[k *n+ j] / u[k *n+ k];
            }
        }

        MPI_Bcast(&l[k+1], n-(k+1), MPI_DOUBLE, k%(nprocs-1)+1, MPI_COMM_WORLD);

		int des = pid-1 - (k+1)%(nprocs-1);
		if (des < 0) des = pid-1 - (k+1)%(nprocs-1) + (nprocs-1);

        for (i=(k+1) + des; i<n; i+=(nprocs-1)) {
            for (j=k+1; j<n; j++) {
                u[i *n+ j] -= u[i *n+ k] * l[j];
            }
        }
    }

    //Enviar U calculado
    for (i=pid-1; i<n; i+=(nprocs-1)) {
        MPI_Send(&u[i*n], n, MPI_DOUBLE, MASTER, TAG, MPI_COMM_WORLD);
    }
}

int main(int argc, char *argv[]){
    int i, j, N, oc, pid, nprocs;
    FILE *in, *out = stdout;
    char input[50];
    double *b, *A, *u, *l, *result;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    while ((oc = getopt(argc, argv, "s:i:")) != -1) {
        switch(oc) {
            case 's':
                N = atoi(optarg);
                break;
            case 'i':
                strcpy(input, optarg);
                break;
        }
    }

    if (MASTER == pid) {
        double stime, ftime, ssend, fsend, srecv, frecv;

		malloc_all(&A, &l, &u, &b, N);
		malloc_vector(&result, N);

        open_file(&in, input);
		read_transpose(A, l, u, b, N, in);

        stime = MPI_Wtime();
        ssend = MPI_Wtime();

        for (i=0; i<N; i++) 
            MPI_Send(&u[i*N], N, MPI_DOUBLE, i%(nprocs-1)+1, TAG, MPI_COMM_WORLD);

		for (i=0; i<N-1; i++) 
			MPI_Bcast(&l[i *N+ i+1], N-(i+1), MPI_DOUBLE, i%(nprocs-1)+1, MPI_COMM_WORLD);

        fsend = MPI_Wtime();
        srecv = MPI_Wtime();

        for (i=0; i<N; i++) 
            MPI_Recv(&u[i*N], N, MPI_DOUBLE, i%(nprocs-1)+1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        frecv = MPI_Wtime();
        ftime = MPI_Wtime();

        if (check_result(A, result, l, u, b, N)) 
            fprintf(out, "( %s, %d ): %lf %lf %lf\n", input, nprocs, ftime-stime, fsend-ssend, frecv-srecv);
        else 
            fprintf(out, "( %s, %d ): ERRO NA VERIFICAÇÃO\n", input, nprocs);

        free_all(A, l, u, b);
    } else {
        decomposeMPI(N, nprocs, pid);
    }


    MPI_Finalize(); 
    return 0;
}

                                                                                  
