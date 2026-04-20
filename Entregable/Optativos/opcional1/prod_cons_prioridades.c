//Codigo realiza por Nicolas Corbal Capas y Hugo Veiga Couselo
// Opcional 1: 3 produtores + 1 consumidor con prioridades e trylock
// Compilación: gcc -Wall -pthread prod_cons_prioridades.c buffer.c -o prod_cons

#include "buffer.h"

struct datos buffers[NP];

void *producer(void *arg) {
    struct datos *args = (struct datos*) arg;
    char nome[20];
    snprintf(nome, 20, "numeros%d.txt", args->prioridade);
    FILE *arq = fopen(nome, "r");
    if (arq == NULL) {
        perror("Erro ao abrir");
        exit(1);
    }
    int suma = 0;
    int item;

    for(int i = 0; i < MAX_ITER; i++){
        sleep(1 + rand()%6); // agarda aleatoria 1-6 s

        //Produccion
        item = produce_item(arq, &suma);
        if(item < 1 || item > 99) break;

        pthread_mutex_lock(&args->mutex);
        while(args->cantidad >= N) pthread_cond_wait(&args->condp, &args->mutex);


        insert_item(item, args);
        printf("[%ld] P%d insire %d\n", tempo_ms(), args->prioridade, item);


        pthread_cond_signal(&args->condc);

        pthread_mutex_unlock(&args->mutex);
    }

    // centinela: inserimos un 0 para marcar fin
    pthread_mutex_lock(&args->mutex);
    while(args->cantidad >= N) pthread_cond_wait(&args->condp, &args->mutex);
    insert_item(0, args);
    pthread_cond_signal(&args->condc);
    pthread_mutex_unlock(&args->mutex);

    printf("\033[96m[%ld] P%d FIN suma = %d\033[0m\n", tempo_ms(), args->prioridade, suma);
    fclose(arq);
    return NULL;
}

void *consumer(void *arg) {
    int sumas[NP] = {0};
    int fin[NP] = {0};
    int item;

    //Bucle hasta que los tres productores terminen
    while(!(fin[0] && fin[1] && fin[2])){

        int feito = 0; //bandera

        // probar buffers por orde de prioridade con trylock
        for(int p = 0; p < NP && !feito; p++){
            if(fin[p]) continue;

            //Trylock para no bloquear al hilo
            if(pthread_mutex_trylock(&buffers[p].mutex) != 0) continue;

            //Comprobacion para ver si hay items
            if(buffers[p].cantidad > 0){

                item = remove_item(&buffers[p]);
                pthread_cond_signal(&buffers[p].condp);
                pthread_mutex_unlock(&buffers[p].mutex);

                if(item == 0){
                    fin[p] = 1;
                    printf("[%ld] Cons fin de P%d\n", tempo_ms(), p+1);
                } else {
                    printf("[%ld] Cons consume %d de P%d\n", tempo_ms(), item, p+1);
                    sumas[p] += item;
                    sleep(1 + rand()%3); // procesado 1-3 s
                }

                feito = 1;//Feito a 1 hace que se salga del bucle para que se vuelva a empezar por el primero
            } else {
                pthread_mutex_unlock(&buffers[p].mutex);
            }
        }

        if(!feito) sched_yield();//Si no se consume se cede la cpu para producción
    }

    printf("\033[95mConsumidor sumas finais:\n");
    for(int p = 0; p < NP; p++){
        printf("P%d: %d\n", p+1, sumas[p]);
    }
    printf("\033[0m");
    return NULL;
}

int main(){
    // inicializacion del randomizador de espera
    srand(time(NULL));

    pthread_t prods[NP], cons_thread;

    for(int p = 0; p < NP; p++){
        buffers[p].limSup = 0;
        buffers[p].limInf = 0;
        buffers[p].cantidad = 0;
        buffers[p].prioridade = p+1;
        pthread_mutex_init(&buffers[p].mutex, 0);
        pthread_cond_init(&buffers[p].condc, 0);
        pthread_cond_init(&buffers[p].condp, 0);
    }

    for(int p = 0; p < NP; p++)
        pthread_create(&prods[p], 0, (void*)producer, &buffers[p]);
    pthread_create(&cons_thread, 0, (void*)consumer, 0);

    for(int p = 0; p < NP; p++) pthread_join(prods[p], NULL);
    pthread_join(cons_thread, NULL);

    for(int p = 0; p < NP; p++){
        pthread_cond_destroy(&buffers[p].condc);
        pthread_cond_destroy(&buffers[p].condp);
        pthread_mutex_destroy(&buffers[p].mutex);
    }
}
