#include "buffer.h"

// Devolve o tempo actual en milisegundos
long tempo_ms(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec*1000L + ts.tv_nsec/1000000L;
}

// Le un enteiro do arquivo e vaino sumando
int produce_item(FILE* arq, int *suma){
    int c;
    if(feof(arq)) return -1;
    if(fscanf(arq,"%d",&c) != 1) return -1;
    *suma += c;
    return c;
}

void insert_item(int item, struct datos *temp){
    if(temp->cantidad<N){
        temp->buffer[temp->limSup] = item;
        temp->limSup = (temp->limSup +1) % N;
        temp->cantidad++;
    }
}

int remove_item(struct datos *temp){
    int item;
    if(temp->cantidad>0){
        item = temp->buffer[temp->limInf];
        temp->buffer[temp->limInf] = 0;
        temp->cantidad--;
        temp->limInf = (temp->limInf +1) % N;
        return item;
    }
    return -1;
}
