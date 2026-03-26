/*Implemente a funçÃO preenche, esboçada abaixo. A função deve preencher os elementos de um vetor v de
tamanho n com um certo valor x. Para acessar os elementos do vetor, não utilize índices, use apenas aritmética
de ponteiros.*/

// *v = endereço do primeiro elemento do vetor "v"
// n = tamanho do vetor "v"
// x = valor a inserir em todas as posições do vetor "v"

#include <stdio.h>
#include <stdlib.h>

void preenche (int *
v, int n, int x){
    int *p = v; 
    for (int i = 0; i < n; i++) {
        *p = x;
        p++; 
    }
}
int main()
{
    
    int tamanho, valor, *p;
    
    printf("Digite o tamanho do vetor: ");
    scanf("%d", &tamanho);
    printf("Informe o valor a preencher o vetor: ");
    scanf("%d", &valor);

    p = malloc(tamanho * sizeof(int));

    preenche(p, tamanho, valor);

    printf("Vetor preenchido: ");
    for (int i = 0; i < tamanho; i++) {
        printf("%d ", p[i]);
    }
    printf("\n");

}