#include <stdio.h>
#include <string.h>

#define MAX_TERRITORIOS 5

/* 
   Struct Territorio:
   Agrupa informações relacionadas a um território do jogo.
   - nome: nome do território
   - cor: cor do exército dominante
   - tropas: quantidade de tropas no território
*/
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

int main() {
    Territorio lista[MAX_TERRITORIOS]; // Vetor com 5 territórios

    // Cadastro
    for (int i = 0; i < MAX_TERRITORIOS; i++) {
        printf("=== Cadastro do Território %d ===\n", i + 1);

        // Leitura do nome
        printf("Digite o nome do território: ");
        scanf("%29s", lista[i].nome);

        // Leitura da cor do exército
        printf("Digite a cor do exército: ");
        scanf("%9s", lista[i].cor);

        // Leitura da quantidade de tropas
        printf("Digite o número de tropas: ");
        scanf("%d", &lista[i].tropas);

        printf("\n");
    }

    // Exibição dos dados cadastrados
    printf("\n===== TERRITÓRIOS REGISTRADOS =====\n");
    for (int i = 0; i < MAX_TERRITORIOS; i++) {
        printf("Território %d:\n", i + 1);
        printf("Nome: %s\n", lista[i].nome);
        printf("Cor do exército: %s\n", lista[i].cor);
        printf("Tropas: %d\n", lista[i].tropas);
        printf("-------------------------------\n");
    }

    return 0;
}
