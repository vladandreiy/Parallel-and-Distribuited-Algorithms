#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Pair {
    long start, end;
} pair_t;

int* arr;
int array_size;

void *incrementArray(void *arg) {
    pair_t pair = *(pair_t*) arg;
    for(int i = pair.start; i < pair.end; i++) {
        for(int j = 0; j < 100; j++) {
            arr[i] += 1;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Specificati dimensiunea array-ului\n");
        exit(-1);
    }

    array_size = atoi(argv[1]);

    arr = malloc(array_size * sizeof(int));
    for (int i = 0; i < array_size; i++) {
        arr[i] = i;
    }

    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }

    void *status;
    long threadsNo = sysconf(_SC_NPROCESSORS_CONF);
    pthread_t threads[threadsNo];
    long id, r, start, end;
    for (id = 0; id < threadsNo; id++) {
        start = id * (double)array_size / threadsNo;
        if(id + 1 < threadsNo)
            end = (id + 1) * (double)array_size / threadsNo;
        else
            end = array_size;
        pair_t *pair = malloc(sizeof(pair_t));
        pair->start = start;
        pair->end = end;
        r = pthread_create(&threads[id], NULL, incrementArray, (void *)pair);
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

    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }

  	pthread_exit(NULL);
}
