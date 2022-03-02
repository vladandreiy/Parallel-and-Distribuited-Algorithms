#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define N 200
#define MAX_NUMBER 10
#define MASTER 0

void compareVectors(int * a, int * b) {
	int i;
	for (i = 0; i < N; i++) {
		if (a[i] != b[i]) {
			printf("Sorted incorrectly at position %d, a[i]:%d, b[i]:%d\n", i, a[i], b[i]);
			return;
		}
	}
	printf("Sorted correctly\n");
}

void displayVector(int * v) {
	int i;
	int displayWidth = 2 + log10(v[N - 1]);
	for (i = 0; i < N; i++) {
		printf("%*i", displayWidth, v[i]);
	}
	printf("\n");
}

int cmp(const void *a, const void *b) {
	int A = *(int*)a;
	int B = *(int*)b;
	return A - B;
}

int main(int argc, char * argv[]) {
	int rank, i, j;
	int nProcesses;
	MPI_Init(&argc, &argv);
	int *pos;
	int *v = (int*)malloc(sizeof(int) * N);
	int *vQSort = (int*)malloc(sizeof(int) * N);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

	int start = rank * ceil((double) N / nProcesses);
	int end = ((rank + 1) * (double) N / nProcesses) < N ?
	          ((rank + 1) * (double) N / nProcesses) : N;
	int dim_proc = end - start;

	if (rank == MASTER) {
		srand(time(0));

		for (int i = 0; i < N; i++) {
			v[i] = rand() % MAX_NUMBER;
		}
		printf("Initial array:\n");
		displayVector(v);
		pos = calloc(N, sizeof(int));
	}
	else {
		pos = calloc(end - start, sizeof(*pos));
	}

	// send the vector to all processes
	MPI_Bcast(v, N, MPI_INT, MASTER, MPI_COMM_WORLD);

	if (rank == 0) {
		for (i = 0; i < N; i++)
			vQSort[i] = v[i];
		qsort(vQSort, N, sizeof(int), cmp);

		for (int i = start; i < end; i++) {
			for (int j = 0; j < N; j++) {
				if (v[j] < v[i] || (v[j] == v[i] && j < i)) {
					pos[i - start]++;
				}
			}
		}
		// recv the new pozitions
		MPI_Gather(pos, dim_proc, MPI_INT, pos, dim_proc, MPI_INT, MASTER, MPI_COMM_WORLD);
		int *res = (int*) calloc(N, sizeof(int));
		for (i = 0; i < N; ++i)
			res[pos[i]] = v[i];
		printf("Sorted array:\n");
		displayVector(res);
		compareVectors(res, vQSort);
	} else {
		// compute the positions
		// send the new positions to process MASTER
		for (int i = start; i < end; i++) {
			for (int j = 0; j < N; j++) {
				if (v[j] < v[i] || (v[j] == v[i] && j < i)) {
					pos[i - start]++;
				}
			}
		}
		MPI_Gather(pos, dim_proc, MPI_INT, NULL, dim_proc, MPI_INT, MASTER, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}
