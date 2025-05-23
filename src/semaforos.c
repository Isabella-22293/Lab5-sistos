#include "compartido.h"
#include <sys/stat.h>   

int available_resources = INIT_RESOURCES;
sem_t resource_sem;
pthread_mutex_t res_mutex = PTHREAD_MUTEX_INITIALIZER;

FILE *logfile;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_event(const char *format, ...) {
    va_list args;
    pthread_mutex_lock(&log_mutex);
    va_start(args, format);
    vfprintf(logfile, format, args);
    fflush(logfile);
    va_end(args);
    pthread_mutex_unlock(&log_mutex);
}

void *thread_func_sem(void *arg) {
    long id = (long)arg;
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        sem_wait(&resource_sem);
        pthread_mutex_lock(&res_mutex);
        available_resources--;
        log_event("[Sem] Thread %ld, iter %d: usó 1 recurso. Quedan %d\n", id, i, available_resources);
        pthread_mutex_unlock(&res_mutex);

        usleep((rand() % 401 + 100) * 1000);

        pthread_mutex_lock(&res_mutex);
        available_resources++;
        log_event("[Sem] Thread %ld, iter %d: devolvió 1 recurso. Quedan %d\n", id, i, available_resources);
        pthread_mutex_unlock(&res_mutex);
        sem_post(&resource_sem);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    mkdir("logs", 0777);
    logfile = fopen(LOG_FILE, "w");
    sem_init(&resource_sem, 0, INIT_RESOURCES);

    pthread_t threads[NUM_THREADS];
    for (long i = 0; i < NUM_THREADS; ++i)
        pthread_create(&threads[i], NULL, thread_func_sem, (void *)i);
    for (int i = 0; i < NUM_THREADS; ++i)
        pthread_join(threads[i], NULL);

    sem_destroy(&resource_sem);
    fclose(logfile);
    return 0;
}