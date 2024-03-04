#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define BUFFER_SIZE 1
#define MAX_LIGHTS 10
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

typedef struct {
    time_t timestamp;
    int lightId;
    int carsPassed;
} TrafficData;

typedef struct {
    TrafficData data[BUFFER_SIZE];
    int in, out;
    sem_t mutex, full, empty;
} BoundedBuffer;

BoundedBuffer buffer;
pthread_mutex_t congestionDataLock = PTHREAD_MUTEX_INITIALIZER;
int congestionData[MAX_LIGHTS];

time_t currentTime; // Global variable to store current time

void initBuffer(BoundedBuffer* buffer) {
    buffer->in = 0;
    buffer->out = 0;
    sem_init(&buffer->mutex, 0, 1);
    sem_init(&buffer->full, 0, 0);
    sem_init(&buffer->empty, 0, BUFFER_SIZE);
}

void produce(BoundedBuffer* buffer) {
    struct timespec currentTimeSpec;
    clock_gettime(CLOCK_REALTIME, &currentTimeSpec);

    for (int i = 0; i < 12; i++) {
        TrafficData newData = {
            .timestamp = currentTimeSpec.tv_sec + (i * 300), // 5 minutes interval
            .lightId = rand() % MAX_LIGHTS + 1,
            .carsPassed = rand() % 100 + 1,
        };

        sem_wait(&buffer->empty);
        sem_wait(&buffer->mutex);

        buffer->data[buffer->in] = newData;
        buffer->in = (buffer->in + 1) % BUFFER_SIZE;

        sem_post(&buffer->mutex);
        sem_post(&buffer->full);

        usleep(rand() % 500000 + 500000); // Simulate delay between measurements
    }
}


void consume(BoundedBuffer* buffer) {
    while (1) {
        sem_wait(&buffer->full);
        sem_wait(&buffer->mutex);

        TrafficData data = buffer->data[buffer->out];
        buffer->out = (buffer->out + 1) % BUFFER_SIZE;

        sem_post(&buffer->mutex);
        sem_post(&buffer->empty);

        pthread_mutex_lock(&congestionDataLock);

        if (congestionData[data.lightId] != 0) {
            congestionData[data.lightId] += data.carsPassed;
        } else {
            congestionData[data.lightId] = data.carsPassed;
        }

        pthread_mutex_unlock(&congestionDataLock);

        char timestampStr[20];
        strftime(timestampStr, sizeof(timestampStr), "%Y-%m-%d %H:%M:%S", localtime(&data.timestamp));
        printf("Data - Time: %s, LightId: %d, CarsPassed: %d\n", timestampStr, data.lightId, data.carsPassed);

        usleep(100000); // Simulate processing time
    }
}

int main() {
    initBuffer(&buffer);

    pthread_t producerThreads[NUM_PRODUCERS];
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producerThreads[i], NULL, (void*)&produce, &buffer);
    }

    pthread_t consumerThreads[NUM_CONSUMERS];
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumerThreads[i], NULL, (void*)&consume, &buffer);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producerThreads[i], NULL);
    }

    pthread_mutex_lock(&congestionDataLock);
    // Notify consumers that no more data will be produced
    pthread_mutex_unlock(&congestionDataLock);

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumerThreads[i], NULL);
    }

    return 0;
}
