#include "buffer.h"

long tempo_ms(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec*1000L + ts.tv_nsec/1000000L;
}

int produce_item(FILE* arq, int *suma, struct item *out){
    int c;
    if(feof(arq)) return -1;
    if(fscanf(arq,"%d",&c) != 1) return -1;
    *suma += c;
    out->valor = c;
    out->t_creacion = tempo_ms();
    out->caducidade = 1 + rand()%12;
    return c;
}

void insert_item(struct item it, struct datos *temp){
    if(temp->cantidad<N){
        //se actualizan indicadores
        temp->buffer[temp->limSup] = it;
        temp->limSup = (temp->limSup +1) % N;
        temp->cantidad++;
    }
}

struct item remove_item(struct datos *temp){
    struct item it = {0, 0, 0};
    if(temp->cantidad>0){
        //se actualizan indicadores
        it = temp->buffer[temp->limInf];
        temp->buffer[temp->limInf].valor = 0;
        temp->cantidad--;
        temp->limInf = (temp->limInf +1) % N;
    }
    return it;
}
