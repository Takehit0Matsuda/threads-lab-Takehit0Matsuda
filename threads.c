#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 3

//Part B
int A = 0;  // Shared variable A
int B = 0;  // Shared variable B
sem_t mutexA;  // Semaphore to control access to variable A
sem_t mutexB;  // Semaphore to control access to variable B

//Part C
sem_t mutex;  // Semaphore to control access to the buffer
sem_t empty;  // Semaphore to count the number of empty slots in the buffer
sem_t full;  // Semaphore to count the number of used slots in the buffer
int buffer[BUFFER_SIZE];  // Buffer with size 3
int head = 0;  // Head index of the buffer
int tail = 0;  // Tail index of the buffer
int outnum = 1;


void append(int value) {
    // Add value to the buffer
    buffer[tail] = value;
    tail = (tail + 1) % BUFFER_SIZE; //0 -> 1 -> 2
}

int take() {
    // Take and return a value from the buffer
    int value = buffer[head];
    head = (head + 1) % BUFFER_SIZE; //0 -> 1 -> 2
    return value;
}

void *producer(void *param) {
    
    // Part A
    intptr_t tid = (intptr_t) param;  // Get thread ID
    printf("I am a Producer, TID: %ld\n", tid);  // Print thread ID

    // Part B
    // Repeat 100 times
    for (int i = 0; i < 100; i++) {
        sem_wait(&mutexA);  // Wait for semaphore to control access to variable A
        A += 1;  // Add 1 to variable A
        //printf ("Producer %ld: add 1\n", tid);
        sem_post(&mutexA);  // Signal semaphore to control access to variable A
        usleep(rand() % 101);  // Sleep for a random time between 0 and 100 microseconds

        sem_wait(&mutexB);  // Wait for semaphore to control access to variable B
        B += 3;  // Add 3 to variable B
        //printf ("Producer %ld: add 3\n", tid);
        sem_post(&mutexB);  // Signal semaphore to control access to variable B
        usleep(rand() % 101);  // Sleep for a random time between 0 and 100 microseconds
    }

    // Part C
    // Repeat 20 times
    for (int i = 0; i < 20; i++) {
        int value = rand() % 100;  // Generate a random value between 0 and 99
        sem_wait(&empty);  // Wait for semaphore to count the number of empty slots in the buffer
        sem_wait(&mutex);  // Wait for semaphore to control access to the buffer
        append(value);  // Append value to the buffer
        printf("Output %d: Producer %ld: produced %d\n", outnum, tid, value);  // Print thread ID and value
        outnum++;
        sem_post(&mutex);  // Signal semaphore to control access to the buffer
        sem_post(&full);  // Signal semaphore to count the number of used slots in the buffer
        usleep(rand() % 101);  // Sleep for a random time between 0 and 100 microseconds
    }
    return NULL;
}

void *consumer(void *param) {
    // Part A
    intptr_t tid = (intptr_t) param;  // Get thread ID
    printf("I am a Consumer, TID: %ld\n", tid);  // Print thread ID

    // Part B
    // Repeat 100 times
    for (int i = 0; i < 100; i++) {
        sem_wait(&mutexB);  // Wait for semaphore to control access to variable B
        B += 3;  // Add 3 to variable B
        //printf ("Consumer %ld: add 3\n", tid);
        sem_post(&mutexB);  // Signal semaphore to control access to variable B
        usleep(rand() % 101);  // Sleep for a random time between 0 and 100 microseconds

        sem_wait(&mutexA);  // Wait for semaphore to control access to variable A
        A += 1;  // Add 1 to variable A
        //printf ("Consumer %ld: add 1\n", tid);
        sem_post(&mutexA);  // Signal semaphore to control access to variable A
        usleep(rand() % 101);  // Sleep for a random time between 0 and 100 microseconds
    }
    
    // Part C
    // Repeat 20 times
    for (int i = 0; i < 20; i++) {
        sem_wait(&full);  // Wait for semaphore to count the number of used slots in the buffer
        sem_wait(&mutex);  // Wait for semaphore to control access to the buffer
        int value = take();  // Take and return a value from the buffer
        printf("Output %d: Consumer %ld: consumed %d\n", outnum, tid, value);  // Print thread ID and value
        outnum++;
        sem_post(&mutex);  // Signal semaphore to control access to the buffer
        sem_post(&empty);  // Signal semaphore to count the number of empty slots in the buffer
        usleep(rand() % 101);  // Sleep for a random time between 0 and 100 microseconds
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // Main function of the program
    if (argc != 2) {
        printf("Usage: %s <number of threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads % 2 != 0) {
        printf("Error: number of threads must be even\n");
        return 1;
    }

    int num_producers = num_threads / 2;
    int num_consumers = num_threads / 2;

    pthread_t threads[num_threads];

    // Initialize semaphores
    sem_init(&mutexA, 0, 1);
    sem_init(&mutexB, 0, 1);
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    printf("Launching %d threads\n", num_threads);

    for (int i = 0; i < num_producers; i++) {
        pthread_create(&threads[i], NULL, producer, (void *) (intptr_t) i);
    }

    for (int i = 0; i < num_consumers; i++) {
        pthread_create(&threads[num_producers + i], NULL, consumer, (void *) (intptr_t) (num_producers + i));
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy semaphores
    sem_destroy(&mutexA);
    sem_destroy(&mutexB);
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    
    // Part B
    printf("A = %d\n", A);
    printf("B = %d\n", B);

    return 0;
}