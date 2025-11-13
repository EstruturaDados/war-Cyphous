#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEFAULT_TERRITOS 5
#define NAME_LEN 30

/* IDs de donos */
#define OWNER_PLAYER 1
#define OWNER_GREEN  2
#define OWNER_NEUTRAL 0

/* Missões */
typedef enum {
    MISSION_DESTROY_GREEN = 1,
    MISSION_CONQUER_3
} Mission;

/* Representa um território */
typedef struct {
    char nome[NAME_LEN];
    int owner;   /* 0 = neutro, 1 = jogador, 2 = verde */
    int tropas;
} Territorio;

/* ---------- Declarações das funções (modularização) ---------- */

/* Alocação e inicialização automática do mapa */
Territorio *alocarMapa(int qtd);
void inicializarMapa(Territorio *mapa, int qtd);

/* Exibição do mapa (não modifica, usa const) */
void exibirMapa(const Territorio *mapa, int qtd);

/* Interface: escolha e validação de índices */
int escolherIndice(const Territorio *mapa, int qtd, const char *prompt);

/* Lógica de combate:
   retorna 1 se houve conquista pelo atacante, 0 caso contrário */
int atacarTerritorios(Territorio *atacante, Territorio *defensor);

/* Missões: sortear, exibir e verificar */
Mission sortearMissao(void);
void exibirMissao(Mission m);
int verificarMissao(const Territorio *mapa, int qtd, Mission m, int conquistados);

/* Liberar memória */
void liberarMapa(Territorio *mapa);

/* ---------- Implementação das funções ---------- */

Territorio *alocarMapa(int qtd) {
    Territorio *mapa = calloc(qtd, sizeof(Territorio));
    if (!mapa) {
        fprintf(stderr, "Erro: falha na alocação de memória.\n");
        exit(EXIT_FAILURE);
    }
    return mapa;
}

/* Inicializa nomes, donos e tropas automaticamente para o jogo */
void inicializarMapa(Territorio *mapa, int qtd) {
    const char *nomes_padrao[] = {
        "Aldea", "Fortaleza", "Monte", "Vale", "Ilha",
        "Porto", "Deserto", "Bosque", "Cidadela", "Refugio"
    };
    int n_nomes = sizeof(nomes_padrao)/sizeof(nomes_padrao[0]);

    /* Distribuição inicial: alguns territórios do jogador, alguns verdes, alguns neutros */
    for (int i = 0; i < qtd; i++) {
        strncpy(mapa[i].nome, nomes_padrao[i % n_nomes], NAME_LEN - 1);
        mapa[i].nome[NAME_LEN - 1] = '\0';

        /* Alterna initial owners para dar diversidade */
        if (i % 3 == 0) mapa[i].owner = OWNER_PLAYER;      /* jogador */
        else if (i % 3 == 1) mapa[i].owner = OWNER_GREEN;   /* verde */
        else mapa[i].owner = OWNER_NEUTRAL;                 /* neutro */

        /* Tropas iniciais: 1 a 5 (suficiente para batalhas) */
        mapa[i].tropas = (rand() % 5) + 1;
    }
}

/* Imprime o mapa atual; não modifica o estado */
void exibirMapa(const Territorio *mapa, int qtd) {
    puts("\n===== MAPA ATUAL =====");
    for (int i = 0; i < qtd; i++) {
        const char *dono;
        if (mapa[i].owner == OWNER_PLAYER) dono = "Jogador";
        else if (mapa[i].owner == OWNER_GREEN) dono = "Verde";
        else dono = "Neutro";

        printf("%d) %s | Dono: %s | Tropas: %d\n", i + 1,
               mapa[i].nome, dono, mapa[i].tropas);
    }
    puts("======================");
}

/* Lê um índice do usuário e valida entre 1..qtd; retorna (0-based) ou -1 em erro */
int escolherIndice(const Territorio *mapa, int qtd, const char *prompt) {
    int escolha = -1;
    printf("%s (1 a %d): ", prompt, qtd);
    if (scanf("%d", &escolha) != 1) {
        while (getchar() != '\n'); /* limpa buffer */
        return -1;
    }
    if (escolha < 1 || escolha > qtd) return -1;
    return escolha - 1;
}

/* Simula ataque entre dois territórios.
   Regras:
   - Cada lado rola um dado 1..6 (rand).
   - Empate favorece o atacante (atacante vence).
   - Se atacante vence: defensor perde 1 tropa.
       - Se defensor.tropas <= 0 -> defensor é conquistado:
         - defensor.owner = atacante.owner
         - defensor.tropas = max(1, atacante->tropas / 2)  (transferência simbólica)
         - atacante perdeu metade das tropas transferidas.
   - Se defensor vence: atacante perde 1 tropa.
   Retorna 1 se houve conquista, 0 caso contrário.
*/
int atacarTerritorios(Territorio *atacante, Territorio *defensor) {
    if (!atacante || !defensor) return 0;

    printf("\n--- Batalha: %s (%d tropas) -> %s (%d tropas) ---\n",
           atacante->nome, atacante->tropas, defensor->nome, defensor->tropas);

    int dadoA = (rand() % 6) + 1;
    int dadoD = (rand() % 6) + 1;
    printf("Dado atacante: %d | Dado defensor: %d\n", dadoA, dadoD);

    if (dadoA >= dadoD) {
        /* atacante vence (empate favorece atacante) */
        printf("Resultado: atacante vence a rodada. Defensor perde 1 tropa.\n");
        defensor->tropas -= 1;
        if (defensor->tropas <= 0) {
            /* conquista */
            int tropas_transfer = atacante->tropas / 2;
            if (tropas_transfer < 1) tropas_transfer = 1;

            int prev_owner = defensor->owner;
            defensor->owner = atacante->owner;
            defensor->tropas = tropas_transfer;

            atacante->tropas -= tropas_transfer;
            if (atacante->tropas < 0) atacante->tropas = 0;

            printf("Território %s conquistado! Agora pertence a %s.\n",
                   defensor->nome,
                   (defensor->owner == OWNER_PLAYER) ? "Jogador" : "Inimigo");

            /* retorna 1 indicando que houve conquista */
            return (prev_owner != defensor->owner) ? 1 : 0;
        }
    } else {
        /* defensor vence a rodada */
        printf("Resultado: defensor resiste. Atacante perde 1 tropa.\n");
        atacante->tropas -= 1;
        if (atacante->tropas < 0) atacante->tropas = 0;
    }
    return 0;
}

/* Sorteia uma missão aleatória dentre as implementadas */
Mission sortearMissao(void) {
    int r = (rand() % 2) + 1;
    return (r == 1) ? MISSION_DESTROY_GREEN : MISSION_CONQUER_3;
}

/* Exibe a missão atual para o jogador */
void exibirMissao(Mission m) {
    puts("\n=== MISSÃO ATUAL ===");
    if (m == MISSION_DESTROY_GREEN) {
        puts("Destruir o exército Verde (nenhum território deve pertencer ao Verde).");
    } else if (m == MISSION_CONQUER_3) {
        puts("Conquistar 3 territórios inimigos durante a partida.");
    } else {
        puts("Missão desconhecida.");
    }
    puts("====================");
}

/* Verifica se a missão foi concluída.
   - Para MISSION_DESTROY_GREEN: retorna 1 se nenhum território pertence a Verde.
   - Para MISSION_CONQUER_3: retorna 1 se 'conquistados' >= 3.
*/
int verificarMissao(const Territorio *mapa, int qtd, Mission m, int conquistados) {
    if (m == MISSION_DESTROY_GREEN) {
        for (int i = 0; i < qtd; i++) {
            if (mapa[i].owner == OWNER_GREEN) return 0;
        }
        return 1;
    } else if (m == MISSION_CONQUER_3) {
        return (conquistados >= 3) ? 1 : 0;
    }
    return 0;
}

void liberarMapa(Territorio *mapa) {
    free(mapa);
}

/* ---------------------- main: loop do jogo ---------------------- */
int main(void) {
    srand((unsigned int)time(NULL));

    int qtd = DEFAULT_TERRITOS;
    Territorio *mapa = alocarMapa(qtd);
    inicializarMapa(mapa, qtd);

    /* Missão sorteada */
    Mission missao = sortearMissao();
    int conquistas_do_jogador = 0;

    printf("Bem-vindo ao WAR - Nível Mestre\n");
    exibirMissao(missao);

    int opcao = -1;
    while (1) {
        printf("\n=== MENU ===\n");
        printf("1 - Atacar\n");
        printf("2 - Verificar Missão\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            printf("Entrada inválida.\n");
            continue;
        }

        if (opcao == 0) {
            puts("Saindo do jogo. Até a próxima!");
            break;
        } else if (opcao == 1) {
            exibirMapa(mapa, qtd);

            int idxA = escolherIndice(mapa, qtd, "Escolha o território atacante");
            if (idxA < 0) {
                puts("Escolha inválida (atacante).");
                continue;
            }
            int idxD = escolherIndice(mapa, qtd, "Escolha o território defensor");
            if (idxD < 0 || idxD == idxA) {
                puts("Escolha inválida (defensor).");
                continue;
            }

            Territorio *at = &mapa[idxA];
            Territorio *df = &mapa[idxD];

            /* Não permitir atacar territórios da mesma cor/dono */
            if (at->owner == df->owner) {
                puts("Não é permitido atacar território com o mesmo dono.");
                continue;
            }

            if (at->owner != OWNER_PLAYER) {
                puts("Apenas territórios do Jogador podem iniciar ataque neste protótipo.");
                continue;
            }

            if (at->tropas <= 0) {
                puts("Território atacante não tem tropas suficientes.");
                continue;
            }

            /* Armazena owner anterior do defensor para detectar conquista do jogador */
            int prev_owner_def = df->owner;
            int houveConquista = atacarTerritorios(at, df);

            /* Se houve conquista e novo dono é o jogador, incrementa contador */
            if (houveConquista && df->owner == OWNER_PLAYER) {
                /* Somente conta se anteriormente não era do jogador */
                if (prev_owner_def != OWNER_PLAYER) {
                    conquistas_do_jogador++;
                    printf("Você conquistou um território! Total conquistados: %d\n", conquistas_do_jogador);
                }
            }

            /* Exibe mapa atualizado e verifica vitória automática */
            exibirMapa(mapa, qtd);
            if (verificarMissao(mapa, qtd, missao, conquistas_do_jogador)) {
                puts("\n*** MISSÃO CONCLUÍDA! VOCÊ VENCEU! ***");
                exibirMissao(missao);
                break;
            }

        } else if (opcao == 2) {
            exibirMissao(missao);
            printf("Progresso atual - Territórios conquistados: %d\n", conquistas_do_jogador);
            if (verificarMissao(mapa, qtd, missao, conquistas_do_jogador)) {
                puts("\n*** MISSÃO CONCLUÍDA! VOCÊ VENCEU! ***");
                break;
            } else {
                puts("Missão ainda não concluída. Continue jogando.");
            }
        } else {
            puts("Opção inválida. Tente novamente.");
        }
    }

    liberarMapa(mapa);
    return 0;
}
