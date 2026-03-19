#include <stdio.h>
/* Escreva um programa que declare variáveis dos tipos int, um float e um char, além de e ponteiros para int,
float, e char. Associe as variáveis aos ponteiros (use &). Modifique os valores de cada variável usando os
ponteiros. Imprima os valores das variáveis antes e após a modificação.
*/
int main() {
    // Valores
    int a = 1;
    float b = 1.99;
    char c = 'A';

    // Ponteiros
    int *p_a = &a;
    float *p_b = &b;
    char *p_c = &c;

    printf("\nValores antes da modificação:\n");
    printf("inteiro: %d\n", a);
    printf("flutuante: %.1f\n", b);
    printf("caractér: %c\n", c);

    // Alterando os valores usando os ponteiros
    *p_a = 10;
    *p_b = 6.9;
    *p_c = 'B';

    printf("\nValores após a modificação:\n");
    printf("inteiro: %d\n", a);
    printf("flutuante: %.1f\n", b);
    printf("caractér: %c\n\n", c);

    return 0;
}