#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define MASTER 0

int main(int argc, char * argv[]) {
	int rank;
	int nProcesses;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	// printf("Hello from %i/%i\n", rank, nProcesses);

	if (rank == MASTER) { // This code is run by a single process
		int polynomialSize, n;
		int x = 5; // valoarea cu care se calculeaza polinomul - f(5)

		/*
			in fisierul de intrare formatul este urmatorul:
			numarul_de_coeficienti
			coeficient x^0
			coeficient x^1
			etc.
		*/
		if(argc < 2) {
			printf("Usage: mpirun -np $threads $executable $filename");
			return -1;
		}
		FILE * polFunctionFile = fopen(argv[1], "rt");
		fscanf(polFunctionFile, "%d", &polynomialSize);
		/*
			in array-ul a se vor salva coeficientii ecuatiei / polinomului
			de exemplu: a = {1, 4, 4} => 1 * (x ^ 2) + 4 * (x ^ 1) + 4 * (x ^ 0)
		*/
		float *a = malloc(sizeof(float) * polynomialSize);
		for (int i = 0; i < polynomialSize; i++) {
			fscanf(polFunctionFile, "%f", &a[i]);
			printf("Read value %f\n", a[i]);
			/*
				Se trimit coeficientii pentru x^1, x^2 etc. proceselor 1, 2 etc.
				Procesul 0 se ocupa de x^0 si are valoarea coeficientului lui x^0
			*/
		}
		for (int i = 1; i < polynomialSize; i++) {
			MPI_Send(a + i, 1, MPI_FLOAT, i, 1, MPI_COMM_WORLD);
		}
		for (int i = polynomialSize; i < nProcesses; i++) {
			int zero = 0;
			MPI_Send(&zero, 1, MPI_FLOAT, i, 1, MPI_COMM_WORLD);
		}

		fclose(polFunctionFile);
		MPI_Send(a, 1, MPI_FLOAT, MASTER + 1, 1, MPI_COMM_WORLD);
		MPI_Send(&x, 1, MPI_INT, MASTER + 1, 1, MPI_COMM_WORLD);

		// Se trimit valorile x si suma partiala (in acest caz valoarea coeficientului lui x^0)
	} else {
		float val, sum;
		int x;

		/*
			se primesc:
			- coeficientul corespunzator procesului (exemplu procesul 1 primeste coeficientul lui x^1)
			- suma partiala
			- valoarea x din f(x)
			si se calculeaza valoarea corespunzatoare pentru c * x^r, r fiind rangul procesului curent
			si c fiind coeficientul lui x^r, si se aduna la suma
		*/
		MPI_Status status;
		MPI_Recv(&val, 1, MPI_FLOAT, MASTER, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&sum, 1, MPI_FLOAT, rank - 1, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&x, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);

		sum += val * pow(x, rank);

		if (rank == nProcesses - 1) {
			printf("Polynom value is %f\n", sum);
		} else {
			// se trimit x si suma partiala catre urmatorul proces
			MPI_Send(&sum, 1, MPI_FLOAT, rank + 1, 1, MPI_COMM_WORLD);
			MPI_Send(&x, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
	return 0;
}
