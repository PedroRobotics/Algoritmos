/*Escreva um programa em linguagem C que solicita ao usuário a quantidade de alunos de uma turma e aloca um
vetor para armazenar a nota semestral de cada um dos alunos (tipo float). Depois de ler as notas e armazena-las
no vetor, apresente a média aritmética das notas da turma. O vetor deve ser alocado dinamicamente na
memória.*/
#include <stdio.h>
#include <stdlib.h>

int main() {
    int quantidade_alunos;
    float *notas, media, soma = 0;

    printf("Digite a quantidade de alunos: ");
    scanf("%d", &quantidade_alunos);

    notas = (float *)malloc(quantidade_alunos * sizeof(float));

    for (int i = 0; i < quantidade_alunos; i++) {
        printf("Digite a nota do aluno %d: ", i + 1);
        scanf("%f", &notas[i]);
        soma += notas[i];
    }

    media = soma / quantidade_alunos;

    printf("A média aritmética das notas é: %.2f\n", media);

    free(notas);

    return 0;
}