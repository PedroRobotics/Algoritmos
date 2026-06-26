/* Para compilar em terminal, digitar gcc lab.c -o lab -pthread -lm */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> // Necessario para o Exercicio 8 (sqrt)

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
Visita v; //! variável global que armazena a última visita realizada
float distancia; //! variável global que armazena a distância para o obstáculo mais próximo

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// --- ESTRUTURAS E PROTÓTIPOS DOS EXERCÍCIOS ---
struct tm matriz_horarios[100][100];
int matriz_visitados[100][100] = {0}; // 0 = nao visitado, 1 = visitado

Ponto lista_visitados_ordenada[10000];
int total_pontos_unicos = 0;

void inserir_ponto_ordenado(int x, int y);
int buscar_indice_mais_proximo(int x_busca, int y_busca);
// ----------------------------------------------

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
        //* gerar valores aleatórios para distância do obstáculo e coordenadas X,Y:
        srand(time(NULL));
        int x = rand() % 100;    // Inteiro entre 0 e 99
        int y = rand() % 100;    // Inteiro entre 0 e 99
        distancia = (float)rand() / (float)(RAND_MAX) * 100.0;  // Float entre 0.0 e 100.0
        
        // * capturar a hora atual * 
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
        
        /** Implementar a leitura dos sensores aqui **/
        
        int x_lido, y_lido; 
        
        visita = ultima_visita();
        x_lido = visita.coordenada.x; 
        y_lido = visita.coordenada.y;
        
        // 1. Guarda a hora na matriz instantânea (Ex 6)
        matriz_horarios[x_lido][y_lido] = *(visita.hora);
        
        // 2. Chama nossa ferramenta para organizar a lista silenciosamente (Ex 7)
        inserir_ponto_ordenado(x_lido, y_lido); 
        
        // 3. Marca como visitado
        matriz_visitados[x_lido][y_lido] = 1; 
        
        /** Finalizar a leitura dos sensores aqui  **/
        
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

//** Implementar todas as funções e estruturas de dados aqui **/

// FERRAMENTA 1: Adiciona um ponto na lista já na ordem certa (lexicográfica)
void inserir_ponto_ordenado(int x, int y) {
    if (matriz_visitados[x][y] == 1) return; // Se ja existe, nao faz nada

    int i = total_pontos_unicos - 1;
    // Empurra os maiores para a direita para abrir espaco
    while (i >= 0 && (lista_visitados_ordenada[i].x > x || 
                     (lista_visitados_ordenada[i].x == x && lista_visitados_ordenada[i].y > y))) {
        lista_visitados_ordenada[i + 1] = lista_visitados_ordenada[i];
        i--;
    }
    // Coloca o novo ponto no espaco vazio
    lista_visitados_ordenada[i + 1].x = x;
    lista_visitados_ordenada[i + 1].y = y;
    total_pontos_unicos++;
}

// FERRAMENTA 2: A Busca Binaria O(log n)
int buscar_indice_mais_proximo(int x_busca, int y_busca) {
    int inicio = 0;
    int fim = total_pontos_unicos - 1;
    int melhor_indice = -1;

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        Ponto p = lista_visitados_ordenada[meio];

        if (p.x > x_busca || (p.x == x_busca && p.y >= y_busca)) {
            melhor_indice = meio; 
            fim = meio - 1;       
        } else {
            inicio = meio + 1;    
        }
    }
    return melhor_indice; 
}

//? Exercicio 6: Acesso O(1)
void horario_visitado(Visita horario) {
    int x, y;

    printf("Informe as coordenadas do ponto visitado (primeiro o x, depois o y): ");
    scanf("%d %d", &x, &y); 

    if(x >= 0 && x < 100 && y >= 0 && y < 100) {
        if (matriz_visitados[x][y] == 1) {
            printf("A ultima visita do ponto (%d, %d) ocorreu as %02d:%02d:%02d.\n",
            x, y,
            matriz_horarios[x][y].tm_hour,
            matriz_horarios[x][y].tm_min,
            matriz_horarios[x][y].tm_sec);
        } else {
            printf("O ponto (%d, %d) nao foi registrado\n", x, y); 
        }
    } else {
        printf("As coordenadas escritas nao estao dentro do limite de (0 a 99)\n"); 
    }
}

//? Exercicio 7: Busca O(log n)
void relatar_ponto_lexicografico() {
    int x, y;
    printf("Informe as coordenadas X e Y da busca (separadas por espaco): ");
    scanf("%d %d", &x, &y);

    int indice = buscar_indice_mais_proximo(x, y);

    if (indice != -1) {
        printf("Senhor, o ponto visitado mais proximo (maior ou igual) a (%d, %d) e: (%d, %d).\n", 
               x, y, 
               lista_visitados_ordenada[indice].x, 
               lista_visitados_ordenada[indice].y);
    } else {
        printf("Nenhum ponto maior ou igual a (%d, %d) foi registrado, senhor.\n", x, y);
    }
}

//? Exercicio 8: Busca por distancia O(n)
void buscar_por_distancia() {
    float d_busca;
    
    printf("Informe a distancia minima 'd' da origem (0,0): ");
    scanf("%f", &d_busca);

    int melhor_x = -1, melhor_y = -1;
    float menor_distancia_valida = 999999.0; 

    for (int i = 0; i < total_pontos_unicos; i++) {
        int x = lista_visitados_ordenada[i].x;
        int y = lista_visitados_ordenada[i].y;
        
        float dist_atual = sqrt(x * x + y * y);

        if (dist_atual >= d_busca && dist_atual < menor_distancia_valida) {
            menor_distancia_valida = dist_atual;
            melhor_x = x;
            melhor_y = y;
        }
    }

    if (melhor_x != -1) {
        printf("Senhor, o ponto visitado mais proximo da origem com distancia >= %.2f e o ponto (%d, %d).\n", 
               d_busca, melhor_x, melhor_y);
        printf("A distancia exata deste ponto ate a origem e: %.2f\n", menor_distancia_valida);
    } else {
        printf("Ainda nao registramos nenhum ponto cuja distancia da origem seja maior ou igual a %.2f, senhor.\n", d_busca);
    }
}

//** **/

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
        printf("\nSelecione uma das opcoes abaixo: \n");
        printf("1. Imprimir distancia para o obstaculo mais proximo\n");
        printf("2. Imprimir os dados do ultimo ponto visitado\n");
        printf("3. Imprimir a distancia media para o obstaculo mais proximo nas primeiras 'x' leituras\n");
        printf("4. Imprimir a distancia media para o obstaculo mais proximo nas ultimas 'x' leituras\n");
        printf("5. Imprimir o horario da primeira visita a um ponto.\n");
        printf("6. Imprimir o horario da ultima visita a um ponto.\n");
        printf("7. Imprimir a coordenada visitada mais proxima de um ponto\n");
        printf("8. Imprimir a coordenada visitada a uma certa distancia da origem\n");
        printf("9. Encerrar o programa\n");
        
        printf("\nDigite uma opcao: ");
        scanf("%d",&opcao);
        switch(opcao){
           case 1: 
              printf("%f\n", distancia_obstaculo()); 
              break;
           case 2: 
              visita = ultima_visita();
              printf("(%d,%d) - %02d:%02d:%02d\n",visita.coordenada.x, visita.coordenada.y, visita.hora->tm_hour, visita.hora->tm_min, visita.hora->tm_sec);
              break;
           /** Implementar as opções 3 a 7 aqui **/   
           
           case 6:
              horario_visitado(visita);
              break;
           case 7:
              relatar_ponto_lexicografico();
              break;
           case 8:
              buscar_por_distancia();
              break;

           /** Fim da implementação das opções 3 a 7  **/
           case 9: 
              pthread_mutex_lock(&mutex);
              thread_status = 0;
              pthread_cond_broadcast(&cond); // Sinalizar todas as threads
              pthread_mutex_unlock(&mutex);
              break;     
           default:
              printf("Opcao invalida\n");
        }
    }

    // Espera as threads finaizarem
    pthread_join(thread_gerar, NULL);
    pthread_join(thread_ler, NULL);

    return 0;
}