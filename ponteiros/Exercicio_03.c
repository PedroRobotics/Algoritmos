/*Escreva uma função chamada troca que troca os valores dos parâmetros recebidos. Sua assinatura deve ser:
void troca(float *a, float *b).
*/

#include <stdio.h>

void troca(float *a, float *b) {
    float temp = *a; 
    *a = *b;         
    *b = temp;       
}

int main() {
    float x = 5.5, y = 10.0;

    printf("\nAntes da troca: x = %.1f, y = %.1f\n", x, y);

    troca(&x, &y);

    printf("Após a troca: x = %.1f, y = %.1f\n\n", x, y);

    return 0;
}
    
