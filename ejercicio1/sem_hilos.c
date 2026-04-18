#include "buffer.h"

struct argumentos{
    struct datos *buffer;
};
int final=0;

void *producer(void *arg) {
    //Inicializacion de variables
    struct datos *args = (struct datos*) arg;
    FILE *arq = fopen("numeros.txt", "r");
    if (arq == NULL) {
        perror("Erro ao abrir lectura.txt");
        exit(1);
    }
    int suma=0;
    int item;


    for(int i = 0; i<MAX_ITER; i++){

        item = produce_item(arq);
        if(!item) break;
        //Se comprueban los semáforos para entrar en la región crítica
        while(args->cantidad>=N);
        insert_item(item, args);
        if(item <1 || item >99) break;
        //Se sale de la región crítica y se actualizan los semáforos
        suma+=item;
        /*for(int i = 0; i < 5; i++){
            printf("Vocal %c: %d\n", "AEIOU"[i], enteros[i]);
        }*/
    }
    final=1;


    printf("Suma del productor es: %d\n",suma);
    

    fclose(arq);
    return NULL;
}

void *consumer(void *arg) {

    struct datos *args = (struct datos*) arg;
    int item;
    int suma=0;
    //sched_yield();

    for(int i = 0; i<MAX_ITER; i++){
        //Se comprueban los semáforos para entrar en la región crítica

        while(args->cantidad<=0 && !final) sleep(1);
        if(final) break;
        // Se accede al buffer compartido
        item = remove_item(args);
        if(item <1 || item >99) break;
        suma+=item;
    }
    printf("Suma del consumidor: %d\n",suma);
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