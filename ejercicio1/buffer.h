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
#define N 10
#define P1 30
#define P2 60
#define MAX_ITER 80
pthread_mutex_t mutex;
pthread_cond_t condc, condp;

struct datos {
    int buffer[N];
    pthread_t *prod;
    pthread_t *cons;
    int limSup;
    int limInf;
    int cantidad;
};

int produce_item(FILE* arq, int *vocal);
void insert_item(int item, struct datos *temp);
int remove_item(struct datos *temp);

#endif