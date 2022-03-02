#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define MASTER 0
#define MAX_ELEMS 8

int main (int argc, char *argv[])
{
    int procs, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int v[MAX_ELEMS];
    for (int i = 0; i < MAX_ELEMS; i++) {
        v[i] = i;
    }
    if (rank == MASTER) {
        printf("Initial vector:");
        for (int i = 0; i < MAX_ELEMS; i++) {
            printf(" %d", v[i]);
        }
        printf("\n");
    }

    int value = rank;
    int sum = v[rank];
    int temp;

    for (int i = 2; i <= procs; i *= 2) {
        if (rank % i == 0 && rank + i / 2 < procs) {
            MPI_Recv(&temp, 1, MPI_INT, rank + i / 2, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += temp;
        }
        else if (rank % (i / 2) == 0 && rank - i / 2 >= 0)
            MPI_Send(&sum, 1, MPI_INT, rank - i / 2, 1, MPI_COMM_WORLD);
    }

    if (rank == MASTER)
        printf("Result = %d\n", sum);

    MPI_Finalize();

}

