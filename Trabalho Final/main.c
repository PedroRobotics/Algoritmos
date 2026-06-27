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

        // Lê os valores dos sensores: distância ao obstáculo mais próximo e última posição visitada
        // Essas variáveis armazenam os dados necessários para registrar uma leitura completa
        float distancia_atual = distancia_obstaculo();
        Visita visita_atual   = ultima_visita();

        void armazenar_leitura(float distancia_lida, int px, int py, struct tm horario);

        if (visita_atual.hora != NULL) {
            armazenar_leitura(distancia_atual,
                              visita_atual.coordenada.x,
                              visita_atual.coordenada.y,
                              *visita_atual.hora);   /* copia o struct tm por valor */
        }

        /**** Finalizar a leitura dos sensores aqui  ****/
        
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}


//**** Implementar todas as funções e estruturas de dados aqui ****/

/* ===================================================================== */
//   CONSTANTE
/* ===================================================================== */
#define MAX_COORD 100   /* coordenadas vão de 0 a 99 (geradas por rand() % 100) */

/* ===================================================================== */
//   VARIÁVEIS E VETORES GLOBAIS
/* ===================================================================== */

/*   1) vetor dinâmico com as distâncias lidas, na ordem em que chegaram
      Índice 0  = leitura mais ANTIGA
      Índice n-1 = leitura mais RECENTE
      Usado nas opções 3 e 4.       */

float *vetor_distancias = NULL;  // distâncias lidas, na ordem em que chegaram
int    num_leituras     = 0;     // quantas leituras já foram armazenadas
int    cap_leituras     = 0;     // capacidade atual do vetor

//  2) HORÁRIO DA PRIMEIRA E DA ÚLTIMA VISITA DE CADA COORDENADA

struct tm primeiro_horario[MAX_COORD][MAX_COORD];
struct tm ultimo_horario  [MAX_COORD][MAX_COORD];
int       ja_visitado     [MAX_COORD][MAX_COORD]; /* 0 = nunca, 1 = já visitado */

//  3) VETOR DINÂMICO COM OS PONTOS DISTINTOS VISITADOS

Ponto *vetor_pontos = NULL;
int    num_pontos   = 0;
int    cap_pontos   = 0;

//  4) ÁRVORE AVL DE PONTOS DISTINTOS (ordenação lexicográfica)
typedef struct no_avl {
   Ponto          ponto;
   int            altura;
   struct no_avl *esq;
   struct no_avl *dir;
} NoAVL;

NoAVL *raiz_avl = NULL;

/* Comparação lexicográfica: primeiro por x; em caso de empate, por y.
   Retorna < 0 se a < b, 0 se a == b, > 0 se a > b. */
int comparar_pontos(Ponto a, Ponto b) {
   if (a.x != b.x) return a.x - b.x;
   return a.y - b.y;
}

int altura_no(NoAVL *no) {
   return (no == NULL) ? 0 : no->altura;
}

int maior(int a, int b) {
   return (a > b) ? a : b;
}

int fator_balanceamento(NoAVL *no) {
   return (no == NULL) ? 0 : altura_no(no->esq) - altura_no(no->dir);
}

NoAVL *criar_no(Ponto p) {
   NoAVL *novo = (NoAVL *) malloc(sizeof(NoAVL));
   novo->ponto  = p;
   novo->altura = 1;
   novo->esq    = NULL;
   novo->dir    = NULL;
   return novo;
}

NoAVL *rotacao_direita(NoAVL *y) {
   NoAVL *x  = y->esq;
   NoAVL *t2 = x->dir;
   x->dir = y;
   y->esq = t2;
   y->altura = 1 + maior(altura_no(y->esq), altura_no(y->dir));
   x->altura = 1 + maior(altura_no(x->esq), altura_no(x->dir));
   return x;
}

NoAVL *rotacao_esquerda(NoAVL *x) {
   NoAVL *y  = x->dir;
   NoAVL *t2 = y->esq;
   y->esq = x;
   x->dir = t2;
   x->altura = 1 + maior(altura_no(x->esq), altura_no(x->dir));
   y->altura = 1 + maior(altura_no(y->esq), altura_no(y->dir));
   return y;
}

/* Inserção em AVL mantendo o balanceamento. Custo: O(log n).
   Pontos repetidos não são duplicados. */
NoAVL *inserir_avl(NoAVL *no, Ponto p) {
   if (no == NULL) return criar_no(p);

   int cmp = comparar_pontos(p, no->ponto);
   if (cmp < 0)      
        no->esq = inserir_avl(no->esq, p);
   else if (cmp > 0)
        no->dir = inserir_avl(no->dir, p);
   else             
        return no;   /* ponto já existe */

   no->altura = 1 + maior(altura_no(no->esq), altura_no(no->dir));

   int fb = fator_balanceamento(no);

   /* Esquerda-Esquerda */
   if (fb > 1 && comparar_pontos(p, no->esq->ponto) < 0)
      return rotacao_direita(no);

   /* Direita-Direita */
   if (fb < -1 && comparar_pontos(p, no->dir->ponto) > 0)
      return rotacao_esquerda(no);

   /* Esquerda-Direita */
   if (fb > 1 && comparar_pontos(p, no->esq->ponto) > 0) {
      no->esq = rotacao_esquerda(no->esq);
      return rotacao_direita(no);
   }
   /* Direita-Esquerda */
   if (fb < -1 && comparar_pontos(p, no->dir->ponto) < 0) {
      no->dir = rotacao_direita(no->dir);
      return rotacao_esquerda(no);
   }
   return no;
}

// Busca o MENOR ponto >= alvo. Retorna NULL se não houver.

NoAVL *teto_avl(NoAVL *no, Ponto alvo) {
   NoAVL *resultado = NULL;
   while (no != NULL) {
      int cmp = comparar_pontos(no->ponto, alvo);
      if (cmp == 0) return no;        // igual ao alvo: é exatamente o teto
      if (cmp > 0) {                  // no->ponto > alvo: candidato
         resultado = no;
         no = no->esq;                // procura algo menor, mas ainda >= alvo
      } else {
         no = no->dir;                // no->ponto < alvo: precisa de maior
      }
   }
   return resultado;
}

// Ordena pontos pela distância à origem

int comparar_por_distancia_origem(const void *a, const void *b) {
   const Ponto *pa = (const Ponto *) a;
   const Ponto *pb = (const Ponto *) b;
   int da = pa->x * pa->x + pa->y * pa->y;
   int db = pb->x * pb->x + pb->y * pb->y;
   if (da < db)
        return -1;
   if (da > db)
        return  1;
   return 0;
}

/* ===================================================================== */
// Armazenamento de Leituras e Visitas
/* ===================================================================== */

void armazenar_leitura(float distancia_lida, int px, int py, struct tm horario) {
   // (1) Guarda a distância no vetor cronológico
   if (num_leituras == cap_leituras) {
      cap_leituras = (cap_leituras == 0) ? 64 : cap_leituras * 2;
      vetor_distancias = (float *) realloc(vetor_distancias,
                                           cap_leituras * sizeof(float));
   }
   vetor_distancias[num_leituras] = distancia_lida;
   num_leituras++;

   // (2) Atualiza horários de visita da coordenada
   if (px >= 0 && px < MAX_COORD && py >= 0 && py < MAX_COORD) {
      if (ja_visitado[px][py] == 0) {
         ja_visitado[px][py] = 1;
         primeiro_horario[px][py] = horario;   // primeira visita

         // Ponto novo: registra na AVL e no vetor de pontos distintos
         Ponto p; p.x = px; p.y = py;
         raiz_avl = inserir_avl(raiz_avl, p);  

         if (num_pontos == cap_pontos) {
            cap_pontos = (cap_pontos == 0) ? 64 : cap_pontos * 2;
            vetor_pontos = (Ponto *) realloc(vetor_pontos,
                                             cap_pontos * sizeof(Ponto));
         }
         vetor_pontos[num_pontos] = p;
         num_pontos++;
      }
      ultimo_horario[px][py] = horario;         // sempre atualiza a última visita //
   }
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

           case 3: {  // Distância média nas x leituras MAIS ANTIGAS
              int x;
              printf("Informe o número de leituras mais antigas (x): ");
              scanf("%d", &x);

              // lê os dados sob o mutex;
              pthread_mutex_lock(&mutex);
              if (num_leituras == 0) {
                 printf("Ainda não há leituras armazenadas.\n");
              } else if (x <= 0) {
                 printf("Valor inválido. Informe x > 0.\n");
              } else {
                 if (x > num_leituras) x = num_leituras;
                 float soma = 0.0;
                 int i;
                 for (i = 0; i < x; i++)
                    soma += vetor_distancias[i];
                 printf("Distância média nas %d leitura(s) mais antiga(s): %.2f\n", x, soma / x);
              }
              pthread_mutex_unlock(&mutex);
              printf("\n");
              break;
           }

           case 4: {  // Distância média nas x leituras MAIS RECENTES
              int x;
              printf("Informe o número de leituras mais recentes (x): ");
              scanf("%d", &x);

              pthread_mutex_lock(&mutex);
              if (num_leituras == 0) {
                 printf("Ainda não há leituras armazenadas.\n");
              } else if (x <= 0) {
                 printf("Valor inválido. Informe x > 0.\n");
              } else {
                 if (x > num_leituras) x = num_leituras;
                 float soma = 0.0;
                 int i;
                 for (i = num_leituras - x; i < num_leituras; i++)
                    soma += vetor_distancias[i];
                 printf("Distância média nas %d leitura(s) mais recente(s): %.2f\n", x, soma / x);
              }
              pthread_mutex_unlock(&mutex);
              printf("\n");
              break;
           }

           case 5: {  // Horário da PRIMEIRA visita a um ponto
              int px, py;
              printf("Informe a coordenada x do ponto: "); scanf("%d", &px);
              printf("Informe a coordenada y do ponto: "); scanf("%d", &py);

              pthread_mutex_lock(&mutex);
              if (px < 0 || px >= MAX_COORD || py < 0 || py >= MAX_COORD
                  || ja_visitado[px][py] == 0) {
                 printf("O ponto (%d,%d) ainda não foi visitado.\n", px, py);
              } else {
                 struct tm h = primeiro_horario[px][py];
                 printf("Primeira visita ao ponto (%d,%d): %02d:%02d:%02d\n",
                        px, py, h.tm_hour, h.tm_min, h.tm_sec);
              }
              pthread_mutex_unlock(&mutex);
              printf("\n");
              break;
           }

           case 6: {  // Horário da ÚLTIMA visita a um ponto
              int px, py;
              printf("Informe a coordenada x do ponto: "); scanf("%d", &px);
              printf("Informe a coordenada y do ponto: "); scanf("%d", &py);

              pthread_mutex_lock(&mutex);
              if (px < 0 || px >= MAX_COORD || py < 0 || py >= MAX_COORD
                  || ja_visitado[px][py] == 0) {
                 printf("O ponto (%d,%d) ainda não foi visitado.\n", px, py);
              } else {
                 struct tm h = ultimo_horario[px][py];
                 printf("Última visita ao ponto (%d,%d): %02d:%02d:%02d\n",
                        px, py, h.tm_hour, h.tm_min, h.tm_sec);
              }
              pthread_mutex_unlock(&mutex);
              printf("\n");
              break;
           }

           case 7: {  // Menor ponto visitado >= (x,y)
              Ponto alvo;
              printf("Informe a coordenada x: "); scanf("%d", &alvo.x);
              printf("Informe a coordenada y: "); scanf("%d", &alvo.y);

              pthread_mutex_lock(&mutex);
              NoAVL *r = teto_avl(raiz_avl, alvo);
              if (r == NULL)
                 printf("Não há ponto visitado maior ou igual a (%d,%d).\n",
                        alvo.x, alvo.y);
              else
                 printf("Menor ponto visitado >= (%d,%d): (%d,%d)\n",
                        alvo.x, alvo.y, r->ponto.x, r->ponto.y);
              pthread_mutex_unlock(&mutex);
              printf("\n");
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
