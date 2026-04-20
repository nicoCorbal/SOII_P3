//Codigo realiza por Nicolas Corbal Capas y Hugo Veiga Couselo
//Compilación: gcc -Wall con_mutex.c buffer.c -o con_mutex -lpthread
#include "buffer.h"
pthread_mutex_t mutex;
pthread_cond_t condc, condp;


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

    int i=0;
    for(i = 0; i<MAX_ITER; i++){

        //Simulador de velocidades
        if(i<=P1){
            sleep(1);
        }else if (i > P1 && i < P2){
            
        } else if (i >= P2){
            sleep(rand()%4);    
        }

        //producción
        item = produce_item(arq);
        if(!item) break;

        //Solicitud entrada en la región crítica
        pthread_mutex_lock(&mutex);
        //Comprobación de hueco
        while(args->cantidad>=N) pthread_cond_wait(&condp,&mutex);

        //insercion
        struct timespec ts = {0, 1000000000}; // 100 ms
        nanosleep(&ts, NULL);
        insert_item(item, args);

        //Aviso de que hay item
        pthread_cond_signal(&condc);
        //Desbloqeuo del mutex, saliendo de región crítica
        pthread_mutex_unlock(&mutex);

        if(item <1 || item >99) break;
 
        suma+=item;
    }

    //Si el archivo tiene menos de 80 elementos, se introduce conidición de parada de consumidor
    if(i<MAX_ITER-1){
        pthread_mutex_lock(&mutex);
        while(args->cantidad!=0) pthread_cond_wait(&condp,&mutex);
        insert_item(0, args);
        pthread_cond_signal(&condc);
        pthread_mutex_unlock(&mutex);
    }



    printf("Suma productor: %d\n",suma);

    fclose(arq);
    return NULL;
}

void *consumer(void *arg) {

    struct datos *args = (struct datos*) arg;
    int item;
    int suma=0;
    //sched_yield();

    for(int i = 0; i<MAX_ITER; i++){
        
        //Simulador de velocidades
        if(i<=P1){
            
        }else if (i > P1 && i < P2){
            sleep(1);
        } else if (i >= P2){
            sleep(rand()%4);    
        }

        //Solicitud entrada en la región crítica
        pthread_mutex_lock(&mutex);
        while(args->cantidad==0) pthread_cond_wait(&condc,&mutex);

        //Comprobación de hueco
        struct timespec ts = {0, 1000000000}; // 100 ms
        nanosleep(&ts, NULL);

        //Consumicion
        item = remove_item(args);

        //Aviso de que hay hueco
        pthread_cond_signal(&condp);

        //Debloqueo de mutex
        pthread_mutex_unlock(&mutex);

        if(item <1 || item >99) break;
        suma+=item;
        /*for(int i = 0; i < 5; i++){
            printf("Vocal %c: %d\n", "AEIOU"[i], enteros[i]);
        }*/
    }
     printf("Suma productor: %d\n",suma);
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

    //Se inicializan mutex y variables de condición
    pthread_mutex_init(&mutex,0);
    pthread_cond_init(&condc,0);
    pthread_cond_init(&condp,0);

    pthread_create(&prod_thread, 0, (void*)producer, &buffer);
    pthread_create(&cons_thread, 0, (void*)consumer, &buffer);

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    //Se liberan mutex y variables de condición
    pthread_cond_destroy(&condc);
    pthread_cond_destroy(&condp);
    pthread_mutex_destroy(&mutex);
}