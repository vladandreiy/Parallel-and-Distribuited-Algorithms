#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define GROUP_SIZE 4

int main (int argc, char *argv[])
{
    int old_size, new_size;
    int old_rank, new_rank;
    int recv_rank;
    MPI_Comm custom_group;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &old_size); // Total number of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &old_rank); // The current process ID / Rank.

    // Split the MPI_COMM_WORLD in small groups.
    new_size = old_size / GROUP_SIZE;
    new_rank = old_rank % new_size;
    MPI_Comm_split(MPI_COMM_WORLD, old_rank / new_size, new_rank, &custom_group);

    printf("Rank [%d] / size [%d] in MPI_COMM_WORLD and rank [%d] / size [%d] in custom group.\n",
           old_rank, old_size, new_rank, new_size);

    // Send the rank to the next process.
    if (new_rank == new_size - 1) {
        MPI_Send(&new_rank, 1, MPI_INT, 0, 1, custom_group);
        // printf("Process %d, group %d, sending rank to process [%d].\n", new_rank, old_rank / GROUP_SIZE, 0);
    }
    else {
        MPI_Send(&new_rank, 1, MPI_INT, new_rank + 1, 1, custom_group);
        // printf("Process %d, group %d, sending rank to process [%d].\n", new_rank, old_rank / GROUP_SIZE, new_rank + 1);
    }

    // Receive the rank.
    MPI_Status status;
    if (new_rank == 0)
        MPI_Recv(&recv_rank, 1, MPI_INT, new_size - 1, 1, custom_group, &status);
    else
        MPI_Recv(&recv_rank, 1, MPI_INT, new_rank - 1, 1, custom_group, &status);

    printf("Process [%d] from group [%d] received [%d].\n", new_rank,
           old_rank / GROUP_SIZE, recv_rank);

    MPI_Finalize();
}

