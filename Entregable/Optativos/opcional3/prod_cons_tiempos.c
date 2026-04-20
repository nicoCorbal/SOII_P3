//Codigo realiza por Nicolas Corbal Capas y Hugo Veiga Couselo
// Opcional 3: tempos fixos por prioridade
//   Produtor: P1 cada 1 s, P2 cada 2 s, P3 cada 3 s.
//   Consumidor: prio1 tarda 3 s, prio2 2 s, prio3 1 s.
// Compilación: gcc -Wall -pthread prod_cons_tiempos.c buffer.c -o prod_cons

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
    struct item it;

    for(int i = 0; i < MAX_ITER; i++){
        sleep(args->prioridade); // tempo fixo: P1=1s, P2=2s, P3=3s
        if(produce_item(arq, &suma, &it) < 1) break;

        pthread_mutex_lock(&args->mutex);
        while(args->cantidad >= N) pthread_cond_wait(&args->condp, &args->mutex);
        insert_item(it, args);
        printf("[%ld] P%d insire %d (caduc %ds)\n",
               tempo_ms(), args->prioridade, it.valor, it.caducidade);
        pthread_cond_signal(&args->condc);
        pthread_mutex_unlock(&args->mutex);
    }

    struct item fin = {0, tempo_ms(), 99};
    pthread_mutex_lock(&args->mutex);
    while(args->cantidad >= N) pthread_cond_wait(&args->condp, &args->mutex);
    insert_item(fin, args);
    pthread_cond_signal(&args->condc);
    pthread_mutex_unlock(&args->mutex);

    printf("[%ld] P%d FIN suma = %d\n", tempo_ms(), args->prioridade, suma);
    fclose(arq);
    return NULL;
}

void *consumer(void *arg) {
    int sumas[NP] = {0};
    int caducados[NP] = {0};
    int fin[NP] = {0};
    struct item it;

    while(!(fin[0] && fin[1] && fin[2])){
        int feito = 0;
        for(int p = 0; p < NP && !feito; p++){
            if(fin[p]) continue;
            if(pthread_mutex_trylock(&buffers[p].mutex) != 0) continue;
            if(buffers[p].cantidad > 0){
                it = remove_item(&buffers[p]);
                pthread_cond_signal(&buffers[p].condp);
                pthread_mutex_unlock(&buffers[p].mutex);
                if(it.valor == 0){
                    fin[p] = 1;
                    printf("[%ld] Cons fin de P%d\n", tempo_ms(), p+1);
                } else if(tempo_ms() - it.t_creacion > (long)it.caducidade * 1000L){
                    printf("[%ld] Cons DESCARTADO %d de P%d (caduc %ds)\n",
                           tempo_ms(), it.valor, p+1, it.caducidade);
                    caducados[p]++;
                } else {


                    printf("[%ld] Cons consume %d de P%d (%ds)\n",
                           tempo_ms(), it.valor, p+1, 3-p);
                    sumas[p] += it.valor;
                    sleep(3-p); // tempo fixo: P1=3s, P2=2s, P3=1s


                    
                }
                feito = 1;
            } else {
                pthread_mutex_unlock(&buffers[p].mutex);
            }
        }
        if(!feito) usleep(10000);
    }

    printf("Consumidor resultado final:\n");
    for(int p = 0; p < NP; p++){
        printf("P%d: suma=%d, caducados=%d\n", p+1, sumas[p], caducados[p]);
    }
    return NULL;
}

int main(){
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
