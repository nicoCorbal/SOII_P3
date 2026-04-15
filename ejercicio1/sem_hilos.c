#include "buffer.h"

struct argumentos{
    struct datos *buffer;
};

void *producer(void *arg) {
    //Inicializacion de variables
    struct datos *args = (struct datos*) arg;
    FILE *arq = fopen("numeros.txt", "r");
    if (arq == NULL) {
        perror("Erro ao abrir lectura.txt");
        exit(1);
    }
    int enteros[99]={0};
    int item;


    for(int i = 0; i<MAX_ITER; i++){

        item = produce_item(arq, enteros);
        if(!item) break;
        //Se comprueban los semáforos para entrar en la región crítica
        while(args->cantidad>=N);
        insert_item(item, args);
        if(item <1 || item >99) break;
        //Se sale de la región crítica y se actualizan los semáforos
        enteros[item-1]++;
        /*for(int i = 0; i < 5; i++){
            printf("Vocal %c: %d\n", "AEIOU"[i], enteros[i]);
        }*/
    }


    printf("Productor vocales finales: \n");
    for(int i = 0; i < 5; i++){
        printf("Entero %d: %d\n", (i+1), enteros[i]);
    }

    fclose(arq);
    return NULL;
}

void *consumer(void *arg) {

    struct datos *args = (struct datos*) arg;
    int item;
    int enteros[99] = {0};
    //sched_yield();

    for(int i = 0; i<MAX_ITER; i++){
        //Se comprueban los semáforos para entrar en la región crítica

        if(args->cantidad<=0) sleep(1);
        // Se accede al buffer compartido
        item = remove_item(args);
        if(item <1 || item >99) break;
        enteros[item-1]++;
    }
    printf("Consumidor vocales finales: \n");
    for(int i = 0; i < 5; i++){
        printf("Entero %d: %d\n", (i+1), enteros[i]);
    }
    return NULL;
}


int main(){

    //inicializacion del randomizador de espera
    srand(time(NULL));

    pthread_t prod_thread, cons_thread;
    struct datos buffer;

    buffer.limSup = 0; // Inicializamos el buffer
    buffer.limInf=0;
    buffer.cantidad=0;

    pthread_create(&prod_thread, NULL, (void*)producer, &buffer);
    pthread_create(&cons_thread, NULL, (void*)consumer, &buffer);

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);
}