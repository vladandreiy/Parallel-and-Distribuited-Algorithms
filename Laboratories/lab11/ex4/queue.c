#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>

typedef struct {
    int size;
    int arr[1000];
} queue;

int main (int argc, char *argv[]) {
    int numtasks, rank;

    // TODO: declare the types and arrays for offsets and block counts

    // TODO: create the MPI data type, using offsets and block counts, and commit the data type
    MPI_Datatype customtype, oldtypes[2];

    int blockcounts[2];
    MPI_Aint offsets[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    // campul size
    offsets[0] = offsetof(queue, size);
    oldtypes[0] = MPI_INT;
    blockcounts[0] = 1;

    // campul arr
    offsets[1] = offsetof(queue, arr);
    oldtypes[1] = MPI_INT;
    blockcounts[1] = 1000;

    // se defineste tipul nou si se comite
    MPI_Type_create_struct(2, blockcounts, offsets, oldtypes, &customtype);
    MPI_Type_commit(&customtype);

    queue q;
    q.size = 0;

    MPI_Request request;
    MPI_Status status;
    int recv_num;

    // First process starts the circle.
    if (rank == 0) {
        // First process starts the circle.
        // Generate a random number and add it in queue.
        // Sends the queue to the next process.
        srand(rank + 125 % (rank-54));
        recv_num = rand();
        q.arr[q.size++] = recv_num;
        printf("Process with rank [%d], appending %d.\n", rank, recv_num);
        MPI_Isend(&q, 1, customtype, rank + 1, 1, MPI_COMM_WORLD, &request);
        MPI_Irecv(&q, 1, customtype, numtasks - 1, 1, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
    } else if (rank == numtasks - 1) {
        // Last process close the circle.
        // Receives the queue from the previous process.
        // Generate a random number and add it in queue.
        // Sends the queue to the first process.
        MPI_Irecv(&q, 1, customtype, rank - 1, 1, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        srand(rank + 125 % (rank-54));
        recv_num = rand();
        q.arr[q.size++] = recv_num;

        printf("Process with rank [%d], appending %d.\n", rank, recv_num);
        MPI_Isend(&q, 1, customtype, 0, 1, MPI_COMM_WORLD, &request);
    } else {
        // Middle process.
        // Receives the queue from the previous process.
        // Generate a random number and add it in queue.
        // Sends the queue to the next process.
        MPI_Irecv(&q, 1, customtype, rank - 1, 1, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);

        srand(rank + 125 % (rank-54));
        recv_num = rand();
        q.arr[q.size++] = recv_num;

        printf("Process with rank [%d], appending %d.\n", rank, recv_num);
        MPI_Isend(&q, 1, customtype, rank + 1, 1, MPI_COMM_WORLD, &request);
    }

    // TODO: Process 0 prints the elements from queue
    if(rank == 0) {
        printf("Process 0: \n")
        for(int i = 0; i < q.size; i++) {
            printf("%d ", q.arr[i]);
        }
        printf("\n");
    }
    // TODO: free the newly created MPI data type
    MPI_Type_free(&customtype);
    MPI_Finalize();
}