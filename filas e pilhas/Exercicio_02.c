/*
   Será que a sequência de parênteses "(()(((()())((((((()))" é válida? Construa um algoritmo
que possibilite a verificação de balanceamento dessa ou qualquer outra sequência de
parênteses. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ************** Início da implementação da pilha usando lista encadeada ******************** */

typedef struct lstItem {
   char dado;
   struct lstItem *next;
}pilhaItem;

typedef struct{
   pilhaItem *topo;	
}pilha;

pilha *criaPilha(){
   pilha *p = malloc(sizeof(pilha));
   p->topo = NULL;
   return p;
}

int pilhaVazia(pilha *p){
   return (p->topo == NULL);
}

void push(pilha *p, char dado){
   pilhaItem *novo = malloc(sizeof(pilhaItem));   
   novo->dado = dado;
   novo->next = p->topo;
   p->topo = novo; 
}

char pop(pilha *p){
   if(pilhaVazia(p)){
      return '\0';
   }
   char dado = p->topo->dado;
   pilhaItem *temp = p->topo;
   p->topo = p->topo->next;
   free(temp);
   return dado;
}

char peek(pilha *p){
   if(p->topo != NULL)
      return p->topo->dado;
   return '\0';
}

void liberaPilha(pilha *p){
   while(!pilhaVazia(p)){
      pop(p);
   }
   free(p);
}

/* ************** Função de verificação de balanceamento ******************** */

int verificaBalanceamento(const char *sequencia){
   pilha *p = criaPilha();
   int i;
   char c;
   
   printf("Analisando sequencia: %s\n\n", sequencia);
   printf("%-10s %-10s %-10s\n", "Caractere", "Acao", "Stack");
   printf("----------------------------------------\n");
   
   for(i = 0; sequencia[i] != '\0'; i++){
      c = sequencia[i];
      
      if(c == '('){
         push(p, c);
         printf("%-10c %-10s ", c, "push");
         pilhaItem *temp = p->topo;
         if(temp == NULL){
            printf("vazia");
         } else {
            while(temp != NULL){
               printf("%c", temp->dado);
               temp = temp->next;
            }
         }
         printf("\n");
      }
      else if(c == ')'){
         if(pilhaVazia(p)){
            printf("%-10c %-10s ", c, "ERRO: stack vazia!");
            printf("\n");
            liberaPilha(p);
            return 0;
         }
         char popped = pop(p);
         printf("%-10c %-10c pop ", c, popped);
         pilhaItem *temp = p->topo;
         if(temp == NULL){
            printf("vazia");
         } else {
            while(temp != NULL){
               printf("%c", temp->dado);
               temp = temp->next;
            }
         }
         printf("\n");
      }
   }
   
   printf("----------------------------------------\n");
   
   if(pilhaVazia(p)){
      printf("Resultado: VALIDA\n");
      printf("Todos os parenteses foram balanceados corretamente.\n");
      liberaPilha(p);
      return 1;
   } else {
      printf("Resultado: INVALIDA\n");
      printf("Sobraram parenteses de abertura sem fechamento.\n");
      pilhaItem *temp = p->topo;
      int count = 0;
      while(temp != NULL){
         count++;
         temp = temp->next;
      }
      printf("Caracteres restantes na pilha: %d\n", count);
      liberaPilha(p);
      return 0;
   }
}

int main()
{
   const char *sequencia = "(()(((()())((((((()))";
   
   printf("========================================\n");
   printf("   VERIFICADOR DE PARENTESES\n");
   printf("========================================\n\n");
   
   int resultado = verificaBalanceamento(sequencia);
   
   printf("\n========================================\n");
   if(resultado){
      printf("   A sequencia E valida!\n");
   } else {
      printf("   A sequencia NAO E valida!\n");
   }
   printf("========================================\n");
   
   printf("\n--- Testes adicionais ---\n\n");
   
   printf("1. (()): ");
   verificaBalanceamento("(()") ? printf("valida\n") : printf("invalida\n");
   
   printf("2. (): ");
   verificaBalanceamento("()") ? printf("valida\n") : printf("invalida\n");
   
   printf("3. ((())): ");
   verificaBalanceamento("((()))") ? printf("valida\n") : printf("invalida\n");
   
   printf("4. ()(: ");
   verificaBalanceamento("()(") ? printf("valida\n") : printf("invalida\n");
   
   return 0;
}