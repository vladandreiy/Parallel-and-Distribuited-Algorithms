#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 2

typedef struct Pair {
	long a, b;
} pair_t;

void *f(void *arg) {
    long id = *(long*) arg;
  	printf("Hello from thread %ld\n", id);
  	pthread_exit(NULL);
}

void *ff(void *arg) {
	pair_t pair = *(pair_t*) arg;
	printf("a = %ld, b = %ld, a*b = %ld\n", pair.a, pair.b, pair.a*pair.b);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	long threadsNo = NUM_THREADS;
	pthread_t threads[threadsNo];
  	int r;
  	long id = 0;
  	void *status;

  	for (id = 0; id < threadsNo; id++) {
  		if(id % 2)
			r = pthread_create(&threads[id], NULL, f, (void *)&id);
		else {
			pair_t pair;
			pair.a = 13;
			pair.b = 15;
			r = pthread_create(&threads[id], NULL, ff, (void *)&pair);
		}
		if (r) {
	  		printf("Eroare la crearea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	for (id = 0; id < threadsNo; id++) {
		r = pthread_join(threads[id], &status);

		if (r) {
	  		printf("Eroare la asteptarea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	pthread_exit(NULL);
}
