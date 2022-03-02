#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "genetic_algorithm.h"

int read_input(sack_object **objects, int *object_count, int *sack_capacity,
               int *generations_count, int argc, char *argv[], int *P) {
    FILE *fp;

    if (argc < 3) {
        fprintf(stderr, "Usage:\n\t./tema1 in_file generations_count\n");
        return 0;
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        return 0;
    }

    if (fscanf(fp, "%d %d", object_count, sack_capacity) < 2) {
        fclose(fp);
        return 0;
    }

    if (*object_count % 10) {
        fclose(fp);
        return 0;
    }

    sack_object *tmp_objects = (sack_object *) calloc(*object_count,
                                                      sizeof(sack_object));

    for (int i = 0; i < *object_count; ++i) {
        if (fscanf(fp, "%d %d", &tmp_objects[i].profit,
                   &tmp_objects[i].weight) < 2) {
            free(objects);
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);

    *generations_count = (int) strtol(argv[2], NULL, 10);

    if (*generations_count == 0) {
        free(tmp_objects);

        return 0;
    }

    if (argc >= 4) {
        *P = (int) strtol(argv[3], NULL, 10);
    }

    *objects = tmp_objects;

    return 1;
}

void print_objects(const sack_object *objects, int object_count) {
    for (int i = 0; i < object_count; ++i) {
        printf("%d %d\n", objects[i].weight, objects[i].profit);
    }
}

void print_generation(const individual *generation, int limit) {
    for (int i = 0; i < limit; ++i) {
        for (int j = 0; j < generation[i].chromosome_length; ++j) {
            printf("%d ", generation[i].chromosomes[j]);
        }

        printf("\n%d - %d\n", i, generation[i].fitness);
    }
}

void print_best_fitness(const individual *generation) {
    printf("%d\n", generation[0].fitness);
}

void
compute_fitness_function(const sack_object *objects, individual *generation,
                         int object_count, int sack_capacity) {
    int weight;
    int profit;

    for (int i = 0; i < object_count; ++i) {
        weight = 0;
        profit = 0;

        for (int j = 0; j < generation[i].chromosome_length; ++j) {
            if (generation[i].chromosomes[j]) {
                weight += objects[j].weight;
                profit += objects[j].profit;
            }
        }

        generation[i].fitness = (weight <= sack_capacity) ? profit : 0;
    }
}

int cmpfunc(const void *a, const void *b) {
    int i;
    individual *first = (individual *) a;
    individual *second = (individual *) b;

    int res = second->fitness - first->fitness; // decreasing by fitness
    if (res == 0) {
        int first_count = 0, second_count = 0;

        for (i = 0; i < first->chromosome_length &&
                    i < second->chromosome_length; ++i) {
            first_count += first->chromosomes[i];
            second_count += second->chromosomes[i];
        }

        res = first_count -
              second_count; // increasing by number of objects in the sack
        if (res == 0) {
            return second->index - first->index;
        }
    }

    return res;
}

void mutate_bit_string_1(const individual *ind, int generation_index) {
    int i, mutation_size;
    int step = 1 + generation_index % (ind->chromosome_length - 2);

    if (ind->index % 2 == 0) {
        // for even-indexed individuals, mutate the first 40% chromosomes by a given step
        mutation_size = ind->chromosome_length * 4 / 10;
        for (i = 0; i < mutation_size; i += step) {
            ind->chromosomes[i] = 1 - ind->chromosomes[i];
        }
    } else {
        // for even-indexed individuals, mutate the last 80% chromosomes by a given step
        mutation_size = ind->chromosome_length * 8 / 10;
        for (i = ind->chromosome_length - mutation_size;
             i < ind->chromosome_length; i += step) {
            ind->chromosomes[i] = 1 - ind->chromosomes[i];
        }
    }
}

void mutate_bit_string_2(const individual *ind, int generation_index) {
    int step = 1 + generation_index % (ind->chromosome_length - 2);

    // mutate all chromosomes by a given step
    for (int i = 0; i < ind->chromosome_length; i += step) {
        ind->chromosomes[i] = 1 - ind->chromosomes[i];
    }
}

void crossover(individual *parent1, individual *child1, int generation_index) {
    individual *parent2 = parent1 + 1;
    individual *child2 = child1 + 1;
    int count = 1 + generation_index % parent1->chromosome_length;

    memcpy(child1->chromosomes, parent1->chromosomes, count * sizeof(int));
    memcpy(child1->chromosomes + count, parent2->chromosomes + count,
           (parent1->chromosome_length - count) * sizeof(int));

    memcpy(child2->chromosomes, parent2->chromosomes, count * sizeof(int));
    memcpy(child2->chromosomes + count, parent1->chromosomes + count,
           (parent1->chromosome_length - count) * sizeof(int));
}

void copy_individual(const individual *from, const individual *to) {
    memcpy(to->chromosomes, from->chromosomes,
           from->chromosome_length * sizeof(int));
}

void free_generation(individual *generation) {
    int i;

    for (i = 0; i < generation->chromosome_length; ++i) {
        free(generation[i].chromosomes);
        generation[i].chromosomes = NULL;
        generation[i].fitness = 0;
    }
}

void merge(individual *gen, int left, int middle, int right) {
    int i, j, k = 0;
    int left_length = middle - left + 1;
    int right_length = right - middle;
    individual *left_array = malloc(left_length * sizeof(individual));
    individual *right_array = malloc(right_length * sizeof(individual));

    memcpy(left_array, gen + left, sizeof(individual) * left_length);
    memcpy(right_array, gen + middle + 1, sizeof(individual) * right_length);

    for (i = 0, j = 0; i < left_length && j < right_length;) {
        if (cmpfunc(&(left_array[i]), &(right_array[j])) < 0) {
            memcpy(gen + left + k, left_array + i, sizeof(individual));
            i++;
        } else {
            memcpy(gen + left + k, right_array + j, sizeof(individual));
            j++;
        }
        k++;
    }
    if (i < left_length)
        memcpy(gen + left + k, left_array + i, sizeof(individual) * (left_length - i));
    if (j < right_length)
        memcpy(gen + left + k, right_array + j, sizeof(individual) * (right_length - j));
    free(left_array);
    free(right_array);
}

void
merge_threads_array(individual *gen, int threads_no, int numbers_per_thread, int object_count, int merging_threads_no) {
    for (int i = 0; i < threads_no; i = i + 2) {
        int left = i * (numbers_per_thread * merging_threads_no);
        // middle = end of first thread array, beginning of next thread's array
        int middle = left + (numbers_per_thread * merging_threads_no) - 1;
        // right = end of 2nd array
        int right = ((i + 2) * numbers_per_thread * merging_threads_no) - 1;
        if (right >= object_count) {
            right = object_count - 1;
        }
        merge(gen, left, middle, right);
    }
    if (threads_no / 2 >= 1) {
        merge_threads_array(gen, threads_no / 2, numbers_per_thread, object_count, merging_threads_no * 2);
    }
}

void merge_sort(individual *gen, int left, int right) {
    if (left < right) {
        int middle = left + (right - left) / 2;
        merge_sort(gen, left, middle);
        merge_sort(gen, middle + 1, right);
        merge(gen, left, middle, right);
    }
}

void *thread_merge_sort(void *arg) {
    merge_arg *args = (merge_arg *) arg;
    int left = args->id * args->object_count / args->P;
    int right;
    if (args->id == args->P - 1)
        right = args->object_count - 1;
    else
        right = (args->id + 1) * (args->object_count / args->P) - 1;
    merge_sort(args->generation, left, right);
    return NULL;
}

void run_genetic_algorithm(const sack_object *objects, int object_count,
                           int generations_count, int sack_capacity, int P) {
    int count, cursor;
    individual *current_generation = (individual *) calloc(object_count,
                                                           sizeof(individual));
    individual *next_generation = (individual *) calloc(object_count,
                                                        sizeof(individual));
    individual *tmp = NULL;

    pthread_t tid[P];
    merge_arg merge_args[P];
    int ret;

    // set initial generation (composed of object_count individuals with a single item in the sack)
    for (int i = 0; i < object_count; ++i) {
        current_generation[i].fitness = 0;
        current_generation[i].chromosomes = (int *) calloc(object_count,
                                                           sizeof(int));
        current_generation[i].chromosomes[i] = 1;
        current_generation[i].index = i;
        current_generation[i].chromosome_length = object_count;

        next_generation[i].fitness = 0;
        next_generation[i].chromosomes = (int *) calloc(object_count,
                                                        sizeof(int));
        next_generation[i].index = i;
        next_generation[i].chromosome_length = object_count;
    }

    // iterate for each generation
    for (int k = 0; k < generations_count; ++k) {
        cursor = 0;

        // compute fitness and sort by it
        compute_fitness_function(objects, current_generation, object_count,
                                 sack_capacity);
        for (long i = 0; i < P; i++) {
            merge_args[i].id = i;
            merge_args[i].object_count = object_count;
            merge_args[i].generation = current_generation;
            merge_args[i].P = P;
            ret = pthread_create(&tid[i], NULL, thread_merge_sort,
                                 (void *) &merge_args[i]);
            if (ret) {
                printf("Error creating thread\n");
                exit(-1);
            }
        }
        for (long i = 0; i < P; i++) {
            ret = pthread_join(tid[i], NULL);
            if (ret) {
                printf("Error joining threads\n");
                exit(-1);
            }
        }
        merge_threads_array(current_generation, P, object_count / P,
                            object_count, 1);

        // keep first 30% children (elite children selection)
        count = object_count * 3 / 10;
        for (int i = 0; i < count; ++i) {
            copy_individual(current_generation + i, next_generation + i);
        }
        cursor = count;

        // mutate first 20% children with the first version of bit string mutation
        count = object_count * 2 / 10;
        for (int i = 0; i < count; ++i) {
            copy_individual(current_generation + i,
                            next_generation + cursor + i);
            mutate_bit_string_1(next_generation + cursor + i, k);
        }
        cursor += count;

        // mutate next 20% children with the second version of bit string mutation
        count = object_count * 2 / 10;
        for (int i = 0; i < count; ++i) {
            copy_individual(current_generation + i + count,
                            next_generation + cursor + i);
            mutate_bit_string_2(next_generation + cursor + i, k);
        }
        cursor += count;

        // crossover first 30% parents with one-point crossover
        // (if there is an odd number of parents, the last one is kept as such)
        count = object_count * 3 / 10;

        if (count % 2 == 1) {
            copy_individual(current_generation + object_count - 1,
                            next_generation + cursor + count - 1);
            count--;
        }

        for (int i = 0; i < count; i += 2) {
            crossover(current_generation + i, next_generation + cursor + i, k);
        }

        // switch to new generation
        tmp = current_generation;
        current_generation = next_generation;
        next_generation = tmp;

        for (int i = 0; i < object_count; ++i) {
            current_generation[i].index = i;
        }

        if (k % 5 == 0)
            print_best_fitness(current_generation);
    }

    compute_fitness_function(objects, current_generation, object_count,
                             sack_capacity);

    for (int i = 0; i < P; i++) {
        merge_args[i].id = i;
        merge_args[i].object_count = object_count;
        merge_args[i].generation = current_generation;
        merge_args[i].P = P;
        ret = pthread_create(&tid[i], NULL, thread_merge_sort,
                             (void *) &merge_args[i]);
        if (ret) {
            printf("Error creating thread\n");
            exit(-1);
        }
    }
    for (int i = 0; i < P; i++) {
        ret = pthread_join(tid[i], NULL);
        if (ret) {
            printf("Error joining threads\n");
            exit(-1);
        }
    }
    merge_threads_array(current_generation, P, object_count / P,
                        object_count, 1);
    print_best_fitness(current_generation);

    free_generation(current_generation);
    free_generation(next_generation);
    free(current_generation);
    free(next_generation);
}