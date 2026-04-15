#include "buffer.h"

void gestion(int numero_de_senhal) {
    switch (numero_de_senhal) {
        case SIGUSR1:
            printf("\nSeñal recibida, soy el proceso con pid: %d\n", getpid());
            // Solo imprimir, no terminar el proceso
            break;
        default:
            printf("Señal desconocida recibida: %d\n", numero_de_senhal);
    }
}

// Funcion auxiliar que devolve 0, 1, 2, 3, 4 segun que vocal sexa l (a,e,i,o,u) e -1 noutro caso
int whatvocal(char letra){
    char l = tolower(letra);
    
    switch (l){
        case 'a':
            return 0;

        case 'e':
            return 1;

        case 'i':
            return 2;

        case 'o':
            return 3;

        case 'u':
            return 4;
    
        default:
            return -1;
    }
}

int produce_item(FILE* arq, int *vocal){
    //Tomamos o caracter correspondente ao punteiro do arquivo e comprobamos que non haxa acabado
    int c;
    if(feof(arq)) return -1;
    fscanf(arq,"%d",&c);

    return c;
}



void insert_item(int item, struct datos *temp){
    if(temp->limSup<0 || temp->limSup>=N) return;
    if(temp->limInf<0 || temp->limInf>=N) return;
    if(temp->cantidad<N){
        struct timespec ts = {0, 1000000000}; // 100 ms
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
        struct timespec ts = {0, 1000000000}; // 100 ms
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
