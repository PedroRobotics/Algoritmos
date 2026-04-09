#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct{
   char nome[30];
   float preco; 
} produto;


typedef struct lstItem{
   produto dado;
   struct lstItem *next;
} lstProduto;



/* Criação da lista: retorna uma lista vazia */
lstProduto *cria_lista(){
   return NULL;
}



/* 
   Imprime a lista.
   Esta função é um exemplo de como percorrer uma lista encadeada.
*/
void print(lstProduto *lista){
   /* "atual" é um ponteiro para o elemento da lista que está sendo examinado. Inicialmente, aponta para o primeiro elemento */
   lstProduto *atual = lista; 

   while(atual!=NULL){
      printf(">>Nome: %s; Preco: %f\n", (*atual).dado.nome, atual->dado.preco);
      atual = atual->next;
   }
}

/*Função que exclui um elemento da lista*/
lstProduto *exclui(lstProduto *lista, produto dado){
    
    /* Procura o elemento anterior ao que será excluído */
    lstProduto *atual = lista;
    lstProduto *anterior = NULL;
    
    while(atual != NULL){
        if(strcmp(atual->dado.nome, dado.nome) == 0){
            break;
        }
        anterior = atual;
        atual = atual->next;
    }
    
    /* Ajusta os ponteiros e libera a memória */
    if(atual != NULL){
        if(anterior == NULL){
            lista = atual->next;
        } else {
            anterior->next = atual->next;
        }
        free(atual);
    }
    
    return lista;
}

/* 
   Procura, em lstProduto, um elemento cujos dados sejam igual ao "dado" passado como parâmetro .
   Retorna um ponteiro para o elemento da lista que contém o referido dado.
*/
lstProduto *busca(lstProduto *lista, produto dado){

}


/* 
   Insere um novo elemento no inicio na lista.
   Retorna um ponteiro para a cabeça da lista.
*/
lstProduto* insere_no_inicio(lstProduto *lista, produto dado){ 
   lstProduto* novo = malloc(sizeof(lstProduto));
   (*novo).dado = dado;
   (*novo).next = lista;
   return novo;
}




/* 
   Insere um novo elemento no final na lista.
   Retorna um ponteiro para a cabeça da lista.

*/
lstProduto* insere_no_fim(lstProduto *lista, produto dado){
  
}




/* 
   Inserção do produto passado no parâmetro "dado" após a posicao que armazena o produto passado no parâmero "posicao".
   Retorna um ponteiro para a cabeça da lista.
*/
lstProduto *insere_apos(lstProduto *lista, produto dado, produto posicao){

}







/* Execução das rotinas necessárias para ler os dados de um novo produto */
produto *novo_produto(){
   produto *novo = malloc(sizeof(produto));
   printf("Nome do produto: ");
   scanf("%s",(*novo).nome);
   printf("Preço: ");
   scanf("%f",&((*novo).preco));
   return novo;
}

int main(){

   lstProduto *lista = cria_lista();
   
   int opcao = 0;   

   while(opcao!=9){   
      /* Imprime o menu de opcoes */
      printf("1 - Inserir no início da lista\n"); 
      printf("2 - Inserir no meio da lista\n");
      printf("3 - Inserir no fim da lista\n");
      printf("4 - Excluir elemento da lista\n");
      printf("5 - Imprimir a lista\n");
      printf("6 - Pesquisar\n");
      printf("9 - Encerrar o programa\n");
      printf("\nDigite a opção desejada: ");
      scanf("%d",&opcao);
      printf("Opcao selecionada %d\n", opcao);

      if(opcao==1){
         produto *novo = novo_produto(); 
         lista = insere_no_inicio(lista, *novo);
      }else 
      if(opcao==2){        
         produto *novo = novo_produto();          
         printf("Informe a posição que o novo elemento deve ocupar:\n");    
         produto *posicao = novo_produto();
         lista = insere_apos(lista, *novo, *posicao);
      }else
      if(opcao==3){
         produto *novo = novo_produto(); 
         lista = insere_no_fim(lista, *novo);
      }else
      if(opcao==5){
         print(lista);
         getchar();
      }else
      if(opcao==6){
         produto *novo = novo_produto();          
         lstProduto *p = busca(lista, *novo);       
         if(p==NULL)
            printf("Produto não encontrado.\n"); 
         else
           printf("Produto encontrado.\n");  
         printf("\n pressione qualquer tecla para continuar\n"  );
         getchar();
         getchar();
      }   
 
	system("clear");
     
   }
   
}