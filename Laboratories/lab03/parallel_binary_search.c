#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define INSIDE -1 // daca numarul cautat este in interiorul intervalului
#define OUTSIDE -2 // daca numarul cautat este in afara intervalului

pthread_barrier_t barrier;

struct my_arg {
	int id;
	int N;
	int P;
	int number;
	int *left;
	int *right;
	int *keep_running;
	int *v;
	int *found;
};

/*
void binary_search() {
	while (keep_running) {
		int mid = left + (right - left) / 2;
		if (left > right) {
			printf("Number not found\n");
			break;
		}

		if (v[mid] == number) {
			keep_running = 0;
			printf("Number found at position %d\n", mid);
		} else if (v[mid] > number) {
			left = mid + 1;
		} else {
			right = mid - 1;
		}
	}
}
*/

void *f(void *arg) {
	struct my_arg* data = (struct my_arg*) arg;
	if (data->number < data->v[*data->left] || data->number > data->v[*data->right - 1]) {
		// printf("Number not in range\n");
		return NULL;
	}
	while (*data->keep_running) {
		int size = ceil(*data->right - *data->left);
		int start = *data->left + data->id * (double) size / data->P;
		int end = *data->left + ((data->id + 1) * (double) size / data->P) < *data->right - 1 ?
		          *data->left + ((data->id + 1) * (double) size / data->P) : *data->right - 1;
		// printf("Id: %d, Size: %d, start: %d, end: %d, v[start]: %d, v[end]: %d\n", data->id, size, start, end, data->v[start], data->v[end]);
		pthread_barrier_wait(&barrier);
		if (data->v[start] == data->number || data->v[end] == data->number) {
			*data->keep_running = 0;
			if (data->v[start] == data->number) {
				*data->found = data->v[start];
				printf("Numar gasit la indexul %d de thread-ul %d!\n", start, data->id);
			}
			else {
				*data->found = data->v[end];
				printf("Numar gasit la indexul %d de thread-ul %d!\n", end, data->id);
			}
		}
		else if (data->v[start] < data->number && data->v[end] > data->number) {
			// printf("Searching in range: [%d %d]\n", start, end);
			*data->left = start;
			*data->right = end;
		}
		if (size / data->P <= 1) {
			*data->keep_running = 0;
			break;
		}
		pthread_barrier_wait(&barrier);
	}

	return NULL;
}

void display_vector(int *v, int size) {
	int i;

	for (i = 0; i < size; i++) {
		printf("%d ", v[i]);
	}

	printf("\n");
}


int main(int argc, char *argv[])
{
	int r, N, P, number, keep_running, left, right;
	int *v;
	int *found;
	void *status;
	pthread_t *threads;
	struct my_arg *arguments;

	if (argc < 4) {
		printf("Usage:\n\t./ex N P number\n");
		return 1;
	}

	N = atoi(argv[1]);
	P = atoi(argv[2]);
	number = atoi(argv[3]);

	if (pthread_barrier_init(&barrier, NULL, P) != 0) {
		printf("Eroare creare bariera.\n");
		return 1;
	}

	keep_running = 1;
	left = 0;
	right = N;

	v = (int*) malloc(N * sizeof(int));
	threads = (pthread_t*) malloc(P * sizeof(pthread_t));
	arguments = (struct my_arg*) malloc(P * sizeof(struct my_arg));
	found = (int*) malloc(P * sizeof(int));

	for (int i = 0; i < N; i++) {
		v[i] = i * 2;
	}

	display_vector(v, N);

	for (int i = 0; i < P; i++) {
		arguments[i].id = i;
		arguments[i].N = N;
		arguments[i].P = P;
		arguments[i].number = number;
		arguments[i].left = &left;
		arguments[i].right = &right;
		arguments[i].keep_running = &keep_running;
		arguments[i].v = v;
		arguments[i].found = found;

		r = pthread_create(&threads[i], NULL, f, &arguments[i]);

		if (r) {
			printf("Eroare la crearea thread-ului %d\n", i);
			exit(-1);
		}
	}

	for (int i = 0; i < P; i++) {
		r = pthread_join(threads[i], &status);

		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
	}

	free(v);
	free(threads);
	free(arguments);
	pthread_barrier_destroy(&barrier);
	return 0;
}
