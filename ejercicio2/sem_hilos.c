#include "buffer.h"
pthread_mutex_t mutex;
pthread_cond_t condc, condp;

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

    int i=0;
    for(i = 0; i<MAX_ITER; i++){
        if (i >= P1 && i < P2){
            
        } else if (i >= P2){
            sleep(rand()%4);    
        }
        item = produce_item(arq, enteros);
        if(!item) break;
        pthread_mutex_lock(&mutex);
        while(args->cantidad>=N) pthread_cond_wait(&condp,&mutex);
        insert_item(item, args);
        pthread_cond_signal(&condc);
        pthread_mutex_unlock(&mutex);

        if(item <1 || item >99) break;
 
        enteros[item-1]++;
        /*for(int i = 0; i < 5; i++){
            printf("Vocal %c: %d\n", "AEIOU"[i], enteros[i]);
        }*/
    }
    if(i<MAX_ITER-1){
        pthread_mutex_lock(&mutex);
        while(args->cantidad!=0) pthread_cond_wait(&condp,&mutex);
        insert_item(0, args);
        pthread_cond_signal(&condc);
        pthread_mutex_unlock(&mutex);
    }



    printf("Productor vocales finales: \n");
    for(int i = 0; i < 10; i++){
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
        if(i>P2){
            sleep(rand()%4);
        }else if(i<P1){
            
        }
        //Se comprueban los semáforos para entrar en la región crítica
        pthread_mutex_lock(&mutex);
        while(args->cantidad==0) pthread_cond_wait(&condc,&mutex);
        item = remove_item(args);
        pthread_cond_signal(&condp);
        pthread_mutex_unlock(&mutex);

        if(item <1 || item >99) break;
        enteros[item-1]++;
        /*for(int i = 0; i < 5; i++){
            printf("Vocal %c: %d\n", "AEIOU"[i], enteros[i]);
        }*/
    }
    printf("Consumidor vocales finales: \n");
    for(int i = 0; i < 10; i++){
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


    pthread_mutex_init(&mutex,0);
    pthread_cond_init(&condc,0);
    pthread_cond_init(&condp,0);

    pthread_create(&prod_thread, 0, (void*)producer, &buffer);
    pthread_create(&cons_thread, 0, (void*)consumer, &buffer);

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    pthread_cond_destroy(&condc);
    pthread_cond_destroy(&condp);
    pthread_mutex_destroy(&mutex);
}