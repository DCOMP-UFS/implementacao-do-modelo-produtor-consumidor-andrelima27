#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define QUEUE_SIZE 10


//gcc at2.c -o at2 -lpthread
//./at2

typedef struct {
    int time[3];
} VectorClock;

VectorClock queue[QUEUE_SIZE];
int front = 0;
int rear = -1;
int count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

void enqueue(VectorClock item) {
    if (count < QUEUE_SIZE) {
        rear = (rear + 1) % QUEUE_SIZE;
        queue[rear] = item;
        count++;
    }
}

VectorClock dequeue() {
    VectorClock item;
    if (count > 0) {
        item = queue[front];
        front = (front + 1) % QUEUE_SIZE;
        count--;
    }
    return item;
}

void *producer(void *param) {
    VectorClock clock;
    while (1) {
        pthread_mutex_lock(&mutex);

        while (count == QUEUE_SIZE) {
            pthread_cond_wait(&empty, &mutex);
        }

       
        for (int i = 0; i < 3; i++) {
            clock.time[i] = rand() % 100;
        }

        enqueue(clock);
        printf("Produced: [%d, %d, %d]\n", clock.time[0], clock.time[1], clock.time[2]);

        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);

        sleep(1);  // Para o cenário onde a fila ficará cheia
        // sleep(2);  // Para o cenário onde a fila ficará vazia
    }
}

void *consumer(void *param) {
    VectorClock clock;
    while (1) {
        pthread_mutex_lock(&mutex);

        while (count == 0) {
            pthread_cond_wait(&full, &mutex);
        }

        clock = dequeue();
        printf("Consumed: [%d, %d, %d]\n", clock.time[0], clock.time[1], clock.time[2]);

        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);

        sleep(3);  // Para o cenário onde a fila ficará cheia
        // sleep(1);  // Para o cenário onde a fila ficará vazia
    }
}

int main() {
    pthread_t producers[3];
    pthread_t consumers[3];

    for (int i = 0; i < 3; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }
    return 0;
}