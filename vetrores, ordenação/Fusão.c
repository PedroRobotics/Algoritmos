#include <stdio.h>
#include <stdlib.h>

/* v1 e v2: ponteiros para os vetores serem fundidos
    t1 e t2: tamanhos de v1 e v2, respectivamente*/
int* merge(int* v1, int t1, int* v2, int t2) {
    int* v3 = (int*) malloc((t1 + t2) * sizeof(int));
    int i,j = 0,k = 0;
    for(i=0; i < t1 + t2; i++) {
        if(v1[j] < v2[k] && j < t1 || k >= t2) {
            v3[i] = v1[j];
            j++;
        } else {
            v3[i] = v2[k];
            k++;
        }
    }
    return v3;
}

int main(){
    int v1[3] = {3, 5, 7}, v2[4] = {2, 4, 6, 8};
    int *v3 = merge(v1, 3, v2, 4); // v3 aponta para um vetor obtido pela fusão de v1 e v2
    int i;
    //imprimir v3
    for (i = 0; i < 7; i++) {
        printf("%d ", v3[i]);
    }
    return 0;
}    