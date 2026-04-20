#ifndef BUFFER_H
#define BUFFER_H
#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <ctype.h>
#define N 10
#define MAX_ITER 80
#define NP 3

// Cada item leva valor, tempo de creación (ms) e caducidade (s)
struct item {
    int valor;
    long t_creacion;
    int caducidade;
};

struct datos {
    struct item buffer[N];
    int limSup;
    int limInf;
    int cantidad;
    int prioridade;
    pthread_mutex_t mutex;
    pthread_cond_t condc, condp;
};

long tempo_ms(void);
int produce_item(FILE* arq, int *suma, struct item *out);
void insert_item(struct item it, struct datos *temp);
struct item remove_item(struct datos *temp);

#endif
