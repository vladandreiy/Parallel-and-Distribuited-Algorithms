#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>

#define COORD_0 0
#define COORD_1 1
#define COORD_2 2
#define COORDINATORS 3

using namespace std;


int main(int argc, char *argv[]) {
    int numtasks, rank;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc < 3) {
        if (rank == 0)
            fprintf(stderr, "Usage: \n\tmpirun -np $processes ./tema3 $size 0/1\n");
        MPI_Finalize();
    }

    // Part 1 - Topology

    int coordinators[COORDINATORS] = {0, 1, 2};
    vector<vector<int>> workers (COORDINATORS);
    FILE *fp;
    int lines_no = 0;
    int coordinator = 0;

    if (rank == COORD_0 || rank == COORD_1 || rank == COORD_2) {
        // Read cluster information from file
        string filename = "cluster" + to_string(rank) + ".txt";
        fp = fopen(filename.c_str(), "r");
        if (fp == NULL) {
            fprintf(stderr, "Unable to open file\n");
            fclose(fp);
            MPI_Finalize();
        }
        if (fscanf(fp, "%d", &lines_no) < 1) {
            fclose(fp);
            MPI_Finalize();
        }

        // Each coordinator stores the workers they are responsible for
        for (int i = 0; i < lines_no; i++) {
            int worker = 0;
            if (fscanf(fp, "%d", &worker) < 1) {
                fclose(fp);
                MPI_Finalize();
            }
            workers[rank].push_back(worker);
        }

        // Send data to other coordinators
        for (int i = 0; i < COORDINATORS; i++) {
            if (i == rank)
                continue;
            MPI_Send(&lines_no, 1, MPI_INT, coordinators[i], 1, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, coordinators[i]);
            MPI_Send(&workers[rank][0], lines_no, MPI_INT, coordinators[i], 1, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, coordinators[i]);
        }

        // Receive data from other coordinators
        for (int i = 0; i < COORDINATORS; i++) {
            if (i == rank)
                continue;
            MPI_Recv(&lines_no, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            workers[i] = vector<int>(lines_no);
            MPI_Recv(&workers[i][0], lines_no, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Send data about each cluster to corresponding workers
        for (size_t i = 0; i < workers[rank].size(); i++) {
            for (int j = 0; j < COORDINATORS; j++) {
                int size = workers[j].size();
                MPI_Send(&size, 1, MPI_INT, workers[rank][i], 1, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, workers[rank][i]);
                MPI_Send(&workers[j][0], workers[j].size(), MPI_INT, workers[rank][i], 1, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, workers[rank][i]);
            }
        }

    }
    else {
        // Worker receives data about topology from coordinator
        for (int i = 0; i < COORDINATORS; i++) {
            MPI_Recv(&lines_no, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            coordinator = status.MPI_SOURCE;
            workers[i] = vector<int>(lines_no);
            MPI_Recv(&workers[i][0], lines_no, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    // Print topology for each process
    string topology;
    topology += to_string(rank) + " -> ";
    for (int i = 0; i < COORDINATORS; i++) {
        topology += to_string(i) + ":";
        for (size_t j = 0; j < workers[i].size() - 1; j++) {
            topology += to_string(workers[i][j]) + ",";
        }
        topology += to_string(workers[i][workers[i].size() - 1]) + " ";
    }
    topology += "\n";
    printf("%s", topology.c_str());


    // Part 2 - Calculations

    MPI_Barrier(MPI_COMM_WORLD);
    int N = strtol(argv[1], NULL, 0);
    vector<int> v(N);
    int workers_no = numtasks - COORDINATORS;
    int chunk_size = N / workers_no;

    if (rank == 0) {
        // Create original vector
        for (int i = 0; i < N; i++) {
            v[i] = i;
        }
        // Split the vector into parts using indices from which a worker
        // will start it's work and stop
        vector<int> workers_indices(workers_no);
        vector<int> workers_size(workers_no);
        workers_indices[0] = 0;
        for (int i = 1; i < workers_no; i++) {
            workers_indices[i] = workers_indices[i - 1] + chunk_size;
            workers_size[i - 1] = workers_indices[i] - workers_indices[i - 1];
        }
        workers_size[workers_no - 1] = N - workers_indices[workers_no - 1];

        // Send the vector to the other processes
        int worker_index = 0;
        for (int i = 0; i < COORDINATORS; i++) {
            if (i == rank) {
                // Direct Send to workers in cluster
                for (size_t j = 0; j < workers[rank].size(); j++) {
                    MPI_Send(&workers_size[worker_index], 1, MPI_INT, workers[rank][j], worker_index, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, workers[rank][j]);
                    MPI_Send(&v[workers_indices[worker_index]], workers_size[worker_index], MPI_INT, workers[rank][j], worker_index, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, workers[rank][j]);
                    worker_index++;
                }
            }
            else {
                // Send to the other Coordinators
                for (size_t j = 0; j < workers[i].size(); j++) {
                    MPI_Send(&workers_size[worker_index], 1, MPI_INT, i, worker_index, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                    MPI_Send(&v[workers_indices[worker_index]], workers_size[worker_index], MPI_INT, i, worker_index, MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", rank, i);
                    worker_index++;
                }
            }
        }
        // Receive modified vector either from workers in cluster or from other Coordinators
        for (int i = 0; i < workers_no; i++) {
            MPI_Recv(&chunk_size, 1, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &status);
            MPI_Recv(&v[workers_indices[i]], chunk_size, MPI_INT, status.MPI_SOURCE, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    else if (rank == COORD_1 || rank == COORD_2) {
        for (size_t i = 0; i < workers[rank].size(); i++) {
            // Receive a part of the original vector from COORD_0
            MPI_Recv(&chunk_size, 1, MPI_INT, COORD_0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            vector<int> recv_v = vector<int>(chunk_size);
            MPI_Recv(&recv_v[0], chunk_size, MPI_INT, COORD_0, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Send a part of the original vector to worker
            MPI_Send(&chunk_size, 1, MPI_INT, workers[rank][i], status.MPI_TAG, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, workers[rank][i]);
            MPI_Send(&recv_v[0], chunk_size, MPI_INT, workers[rank][i], status.MPI_TAG, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, workers[rank][i]);

        }
        for (size_t i = 0; i < workers[rank].size(); i++) {
            // Receive modified vector from worker
            MPI_Recv(&chunk_size, 1, MPI_INT, workers[rank][i], MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            vector<int> recv_v = vector<int>(chunk_size);
            MPI_Recv(&recv_v[0], chunk_size, MPI_INT, workers[rank][i], status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Send to COORD_0
            MPI_Send(&chunk_size, 1, MPI_INT, COORD_0, status.MPI_TAG, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, COORD_0);
            MPI_Send(&recv_v[0], chunk_size, MPI_INT, COORD_0, status.MPI_TAG, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, COORD_0);
        }
    }
    else {
        // Receive a part of the original vector from coordinator
        MPI_Recv(&chunk_size, 1, MPI_INT, coordinator, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        vector<int> recv_v = vector<int>(chunk_size);
        MPI_Recv(&recv_v[0], chunk_size, MPI_INT, coordinator, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Modify the vector
        for (int i = 0; i < chunk_size; i++) {
            recv_v[i] = 2 * recv_v[i];
        }

        // Send modified vector to coordinator
        MPI_Send(&chunk_size, 1, MPI_INT, coordinator, status.MPI_TAG, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, coordinator);
        MPI_Send(&recv_v[0], chunk_size, MPI_INT, coordinator, status.MPI_TAG, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, coordinator);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Print the final, modified vector
    if (rank == 0) {
        printf("Rezultat: ");
        for (int i = 0; i < N; i++) {
            printf("%d ", v[i]);
        }
        printf("\n");
    }
    MPI_Finalize();
}
