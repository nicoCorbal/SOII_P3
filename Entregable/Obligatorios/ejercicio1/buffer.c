#include "buffer.h"


int produce_item(FILE* arq){
    int c;
    if (fscanf(arq, "%d", &c) != 1) {
        return -1;  // EOF o error de formato
    }
    return c;
}



void insert_item(int item, struct datos *temp){
    if(temp->limSup<0 || temp->limSup>=N) return;
    if(temp->limInf<0 || temp->limInf>=N) return;
    if(temp->cantidad<N){
        //se actualizan indicadores
        struct timespec ts = {0, 10000000000}; // 100 ms
        nanosleep(&ts, NULL);
        temp->buffer[temp->limSup] = item;
        temp->limSup = (temp->limSup +1) % N;
        temp->cantidad++;
    }else{
        printf("Buffer lleno, no se puede insertar el item: %d\n", item);
        return;
    }
}

int remove_item(struct datos *temp){
    int item;
    if(temp->limSup<0 || temp->limSup>=N) return -1;
    if(temp->limInf<0 || temp->limInf>=N) return -1;
    if(temp->cantidad>0){
        //se actualizan indicadores
        struct timespec ts = {0, 10000000000}; // 100 ms
        nanosleep(&ts, NULL);
        item = temp->buffer[temp->limInf];
        temp->cantidad--;
        temp->limInf = (temp->limInf +1) % N;
        return item;
        
    }else{
        printf("Buffer vacío, no se puede remover ningún item\n");
        //dormir();
        return -1;
    }
    printf("Kernel panic\n");
    return '\0'; // Devuelve un carácter nulo si no se pudo remover ningún item
}
