#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>   


// Parámetros ajustables
#define NUM_THREADS    5
#define NUM_ITERATIONS 10
#define INIT_RESOURCES 3
#define LOG_FILE "logs/bitacora.txt"

extern FILE *logfile;
extern pthread_mutex_t log_mutex;

void log_event(const char *format, ...);

#endif 