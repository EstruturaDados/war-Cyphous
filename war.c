#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

/* Cadastro simples */
void cadastrar(Territorio *mapa, int qtd) {
    for (int i = 0; i < qtd; i++) {
        printf("=== Território %d ===\n", i + 1);

        printf("Nome: ");
        scanf("%29s", mapa[i].nome);

        printf("Cor do exército: ");
        scanf("%9s", mapa[i].cor);

        printf("Tropas: ");
        scanf("%d", &mapa[i].tropas);

        printf("\n");
    }
}

/* Exibição simples */
void exibirMapa(Territorio *mapa, int qtd) {
    printf("\n===== ESTADO DO MAPA =====\n");
    for (int i = 0; i < qtd; i++) {
        printf("Território %d:\n", i + 1);
        printf("Nome: %s\n", mapa[i].nome);
        printf("Cor: %s\n", mapa[i].cor);
        printf("Tropas: %d\n", mapa[i].tropas);
        printf("------------------------------\n");
    }
}

/* Ataque com regras pedidas */
void atacar(Territorio *atacante, Territorio *defensor) {
    printf("\n%s (%s) está atacando %s (%s)\n",
           atacante->nome, atacante->cor,
           defensor->nome, defensor->cor);

    int dadoA = (rand() % 6) + 1;
    int dadoD = (rand() % 6) + 1;

    printf("Dado do atacante: %d\n", dadoA);
    printf("Dado do defensor: %d\n", dadoD);

    if (dadoA >= dadoD) { // empate favorece atacante
        printf("Atacante vence essa rodada!\n");
        defensor->tropas--;

        if (defensor->tropas <= 0) {
            printf("Território conquistado!\n");
            strcpy(defensor->cor, atacante->cor);
            defensor->tropas = 1; // assume 1 tropa mínima
        }

    } else {
        printf("Defensor resiste! Atacante perde 1 tropa.\n");
        atacante->tropas--;
        if (atacante->tropas < 0)
            atacante->tropas = 0;
    }
}

int main() {
    srand(time(NULL));

    int qtd = 5;
    Territorio *mapa = calloc(qtd, sizeof(Territorio));

    cadastrar(mapa, qtd);

    int op = 0;
    while (op != 3) {
        printf("\n=== MENU ===\n");
        printf("1 - Exibir mapa\n");
        printf("2 - Atacar\n");
        printf("3 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &op);

        if (op == 1) {
            exibirMapa(mapa, qtd);
        }

        else if (op == 2) {
            int a, d;
            exibirMapa(mapa, qtd);

            printf("Escolha atacante (1 a 5): ");
            scanf("%d", &a);

            printf("Escolha defensor (1 a 5): ");
            scanf("%d", &d);

            if (a < 1 || a > qtd || d < 1 || d > qtd || a == d) {
                printf("Seleção inválida.\n");
                continue;
            }

            Territorio *atacante = &mapa[a - 1];
            Territorio *defensor  = &mapa[d - 1];

            if (strcmp(atacante->cor, defensor->cor) == 0) {
                printf("Não pode atacar território da mesma cor.\n");
                continue;
            }

            if (atacante->tropas <= 0) {
                printf("O atacante não tem tropas suficientes!\n");
                continue;
            }

            atacar(atacante, defensor);
        }
    }

    free(mapa);
    return 0;
}
