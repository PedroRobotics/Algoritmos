/* Para compilar em terminal, digitar gcc trabalho_cac3202_veiculo_pontos.c -o trabalho_cac3202_veiculo_pontos -pthread */


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


//Struct que representa uma coordenada (X,Y)
typedef struct ponto{
   int x; //coordenada X
   int y; // coordenada Y
}Ponto;


//Struct que representa uma visita a um determinado ponto em uma determinada hora
typedef struct visita{
   Ponto coordenada;
   struct tm *hora; //hora em que a visita aconteceu   
}Visita;


int thread_status = 1;
Visita v; //variável global que armazena a última visita realizada
float distancia; //variável global que armazena a distância para o obstáculo mais próximo

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


//função que retorna os dados da última visita
Visita ultima_visita(){
   return v;
}


//função que retorna a distância atual para o obstáculo mais próximo
float distancia_obstaculo(){
   return distancia;
}


// Função que simula a geração de valores de sensores
void* gerar_valores_de_sensores(void* arg) {
         
    while (thread_status==1) {
        //*** gerar valores aleatórios para distância do obstáculo e coordenadas X,Y:
        srand(time(NULL));
        int x = rand() % 100;    // Inteiro entre 0 e 99
        int y = rand() % 100;    // Inteiro entre 0 e 99
        distancia = (float)rand() / (float)(RAND_MAX) * 100.0;  // Float entre 0.0 e 100.0

        
        // *** capturar a hora atual *** 
        time_t agora;
        time(&agora); //obter o tempo atual
        
        
        
        v.coordenada.x = x;
        v.coordenada.y = y;
        v.hora = localtime(&agora);  
        
        // Sinaliza a thread de leitura de sensores pode executar
        pthread_mutex_lock(&mutex);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);

        
        sleep(1);  // Aguarda um segundo até a próxima leitura de sensores
    }
    return NULL;
}



// Função que simula a leitura de valores de sensores
void* ler_valores_de_sensores(void* arg) {
    Visita visita;     
    while (thread_status == 1) {
        // Espera até receber o sinal da thread de geração de valores
        pthread_mutex_lock(&mutex);

        // Verifica se o programa ainda deve continuar antes de esperar pelo sinal
        if (thread_status == 0) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        pthread_cond_wait(&cond, &mutex);
        
        if (thread_status == 0) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        
        //*** As três linhas abaixo ilustram a leitura de sensores. Descomentar para testar 
        //printf("Distância para o obstáculo mais próximo: %f\n", distancia_obstaculo());
        //Visita visita = ultima_visita();
        //printf("(%d,%d) - %02d:%02d:%02d\n",visita.coordenada.x, visita.coordenada.y, visita.hora->tm_hour, visita.hora->tm_min, visita.hora->tm_sec);
        
        /**** Implementar a leitura dos sensores aqui ****/
        adicionar_distancia_historico(distancia_obstaculo());

        /**** Finalizar a leitura dos sensores aqui  ****/
        
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}


//**** Implementar todas as funções e estruturas de dados aqui ****/

#define MAX_HISTORICO 1000

typedef struct {
    float distancias[MAX_HISTORICO];
    int inicio;
    int tamanho;
} HistoricoDistancias;

HistoricoDistancias historico = {{0}, 0, 0};
pthread_mutex_t mutex_historico = PTHREAD_MUTEX_INITIALIZER;

void adicionar_distancia_historico(float d) {
    pthread_mutex_lock(&mutex_historico);
    if (historico.tamanho < MAX_HISTORICO) {
        historico.distancias[(historico.inicio + historico.tamanho) % MAX_HISTORICO] = d;
        historico.tamanho++;
    } else {
        historico.distancias[historico.inicio] = d;
        historico.inicio = (historico.inicio + 1) % MAX_HISTORICO;
    }
    pthread_mutex_unlock(&mutex_historico);
}

float media_x_leituras_antigas(int x) {
    if (x <= 0) return 0;

    pthread_mutex_lock(&mutex_historico);

    if (x > historico.tamanho) {
        x = historico.tamanho;
    }

    float soma = 0;
    for (int i = 0; i < x; i++) {
        soma += historico.distancias[(historico.inicio + i) % MAX_HISTORICO];
    }

    pthread_mutex_unlock(&mutex_historico);

    return x > 0 ? soma / x : 0;
}



//**** 	 ****/


int main() {
    pthread_t thread_gerar, thread_ler;
    Visita visita; 
    int opcao=0;
    
    // Criação da thread que simula geração de valores de sensores
    if (pthread_create(&thread_gerar, NULL, gerar_valores_de_sensores, NULL) != 0) {
        perror("Erro ao criar thread");
        return 1;
    }
    
    // Criação da thread que lê de valores de sensores simulados
    if (pthread_create(&thread_ler, NULL, ler_valores_de_sensores, NULL) != 0) {
        perror("Erro ao criar thread");
        return 1;
    }

    // Função principal aguarda a tecla ENTER para imprimir o contador
    while (opcao!=9) {
        printf("Selecione uma das opções abaixo: \n");
        printf("1. Imprimir distância para o obstáculo mais próximo\n");
        printf("2. Imprimir os dados do último ponto visitado\n");
        printf("3. Imprimir a distância média para o obstáculo mais próximo nas primeiras 'x' leituras\n");
        printf("4. Imprimir a distância média para o obstáculo mais próximo nas últimas 'x' leituras\n");
        printf("5. Imprimir o horário da primeira visita a um ponto.\n");
        printf("6. Imprimir o horário da ultima visita a um ponto.\n");
        printf("7. Imprimir a coordenada visitada mais próxima de um ponto\n");
        printf("8. Imprimir a coordenada visitada a uma certa distância da origem\n");
        printf("9. Encerrar o programa\n");
        

        printf("\nDigite uma opção: ");
        scanf("%d",&opcao);
        switch(opcao){
           case 1: 
              printf("%f\n", distancia_obstaculo()); 
              break;
           case 2: 
              visita = ultima_visita();
              printf("(%d,%d) - %02d:%02d:%02d\n",visita.coordenada.x, visita.coordenada.y, visita.hora->tm_hour, visita.hora->tm_min, visita.hora->tm_sec);
              break;
           /**** Implementar as opções 3 a 7 aqui ****/
           case 3: {
              int x;
              printf("Digite o valor de x: ");
              scanf("%d", &x);
              float media = media_x_leituras_antigas(x);
              printf("Distância média nas %d leituras mais antigas: %.2f\n", x, media);
              break;
           }
           /**** Fim da implementação das opções 3 a 7  ****/
           case 9: 
              pthread_mutex_lock(&mutex);
              thread_status = 0;
              pthread_cond_broadcast(&cond); // Sinalizar todas as threads
              pthread_mutex_unlock(&mutex);
              break;     
           default:
              printf("Opção inválida\n");
        }

        
        
        
        
    }

    // Espera as threads finaizarem
    pthread_join(thread_gerar, NULL);
    pthread_join(thread_ler, NULL);

    return 0;
}
