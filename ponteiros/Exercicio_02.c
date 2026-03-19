/*Crie um programa que contenha um vetor de inteiros contendo 5 elementos. Utilizando apenas aritmética de
ponteiros, leia esse vetor do teclado e imprima o dobro de cada valor lido.*/

#include <stdio.h>

int main() {
    int numeros[5];
    int *p = numeros;

    printf("Digite 5 números inteiros:\n");
    
    for (int i = 0; i < 5; i++) {
        scanf("%d", p + i);
    }
    
    printf("\nO dobro dos números digitados é:\n");

    for (int i = 0; i < 5; i++) {
        printf("%d ", 2 * (*(p + i)));
    }
    
    printf("\n");

    return 0;
}