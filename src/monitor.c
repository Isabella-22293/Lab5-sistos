#include "compartido.h"
#include <sys/stat.h>   

int available_resources = INIT_RESOURCES;
pthread_mutex_t mon_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mon_cond   = PTHREAD_COND_INITIALIZER;

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

int decrease_count(int count) {
    pthread_mutex_lock(&mon_mutex);
    while (available_resources < count) {
        log_event("[Mon] Esperando %d recursos. Disponibles: %d\n", count, available_resources);
        pthread_cond_wait(&mon_cond, &mon_mutex);
    }
    available_resources -= count;
    log_event("[Mon] Consumió %d recursos. Quedan %d\n", count, available_resources);
    pthread_mutex_unlock(&mon_mutex);
    return 0;
}

int increase_count(int count) {
    pthread_mutex_lock(&mon_mutex);
    available_resources += count;
    log_event("[Mon] Devolvió %d recursos. Disponibles: %d\n", count, available_resources);
    pthread_cond_broadcast(&mon_cond);
    pthread_mutex_unlock(&mon_mutex);
    return 0;
}

void *thread_func_mon(void *arg) {
    long id = (long)arg;
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        int want = (rand() % INIT_RESOURCES) + 1;
        log_event("[Mon] Thread %ld, iter %d: solicita %d\n", id, i, want);
        decrease_count(want);
        usleep((rand() % 401 + 100) * 1000);
        increase_count(want);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    mkdir("logs", 0777);
    logfile = fopen(LOG_FILE, "w");

    pthread_t threads[NUM_THREADS];
    for (long i = 0; i < NUM_THREADS; ++i)
        pthread_create(&threads[i], NULL, thread_func_mon, (void *)i);
    for (int i = 0; i < NUM_THREADS; ++i)
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&mon_mutex);
    pthread_cond_destroy(&mon_cond);
    fclose(logfile);
    return 0;
}