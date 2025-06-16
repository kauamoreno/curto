#include <stdio.h>
#include <stdlib.h>
#include "17151550.h"
#include "../baralho.h"
#include "../mao.h"

#define MANILHA_EXTRA 20 // Valor fictício para distinguir manilha

typedef struct {
    int id;
    int num_jogadores;
    Carta mao[6];
    int n_mao;
    Valor manilha;
    Naipe naipe_virado;
    int rodada;
    int minhas_vitorias;
    int minha_aposta;
} JogadorState;

static JogadorState jogador;

const char* nome_17151550() {
    return "Kaua M 17151550";
}

void iniciar_17151550(const int id, const int num_jogadores) {
    jogador.id = id;
    jogador.num_jogadores = num_jogadores;
}

static int eh_manilha(Carta c) {
    return c.valor == jogador.manilha;
}

static void atualizar_manilhas() {
    for (int i = 0; i < jogador.n_mao; i++) {
        if (eh_manilha(jogador.mao[i])) {

            //Peso Manilha = 20 + Naipe
            jogador.mao[i].valor = MANILHA_EXTRA + jogador.mao[i].naipe;
        }
    }
}

void nova_rodada_17151550(const int rodada, const Carta carta_virada, const int n_cartas, Carta* mao) {
    jogador.rodada = rodada;
    jogador.n_mao = n_cartas;
    jogador.naipe_virado = carta_virada.naipe;
    jogador.manilha = definir_manilha(carta_virada);
    jogador.minhas_vitorias = 0;

    //Guarda a minha mão e verifica manilhas
    for (int i = 0; i < n_cartas; i++) {
        jogador.mao[i] = mao[i];
    }
    atualizar_manilhas();
}

// Estratégia: aposta baseada no número de manilhas + distribuição das cartas
int apostar_17151550(const int* apostas) {
    int manilhas = 0;
    int cartas_fortes = 0; //TRES ou DOIS

    for (int i = 0; i < jogador.n_mao; i++) {
        if (jogador.mao[i].valor >= MANILHA_EXTRA) manilhas++;
        else if (jogador.mao[i].valor == TRES || jogador.mao[i].valor == DOIS) cartas_fortes++;
    }

    // Estimação baseada em distribuição de força
    // Manilha é contada como vitoria e a cada 2 fortes uma vitoria
    int aposta = manilhas + (cartas_fortes / 2);
    if (aposta == 0) aposta = 1;
    if (aposta > jogador.n_mao) aposta = jogador.n_mao;

    jogador.minha_aposta = aposta;
    return aposta;
}

// Função que busca a pior carta da mão
static int carta_mais_fraca() {
    int indice = -1;
    Valor menor = 100;

    for (int i = 0; i < jogador.n_mao; i++) {
        if (carta_foi_usada(jogador.mao[i])) continue;
        if (jogador.mao[i].valor < menor) {
            menor = jogador.mao[i].valor;
            indice = i;
        }
    }

    return indice;
}

// Função que busca a melhor carta que provavelmente vence
static int melhor_chance(const Carta* mesa, int num_mesa) {
    int indice = -1;
    Valor maior = -1;

    for (int i = 0; i < jogador.n_mao; i++) {
        if (carta_foi_usada(jogador.mao[i])) continue;

        int vence = 1;
        for (int j = 0; j < num_mesa; j++) {
            if (comparar_cartas(jogador.mao[i], mesa[j], jogador.manilha) <= 0) {
                vence = 0;
                break;
            }
        }

        if (vence && jogador.mao[i].valor > maior) {
            maior = jogador.mao[i].valor;
            indice = i;
        }
    }

    return indice;
}

int jogar_17151550(const Carta* mesa, const int num_na_mesa, const int vitorias) {
    jogador.minhas_vitorias = vitorias;

    //Se já alcancei a aposta, não preciso vencer mais
    if (vitorias >= jogador.minha_aposta) {
        int idx = carta_mais_fraca();
        if (idx != -1) {
            jogador.mao[idx] = USADA;
            return idx;
        }
    }

    // Caso contrário, tento jogar uma carta que provavelmente vence
    int idx = melhor_chance(mesa, num_na_mesa);
    if (idx != -1) {
        jogador.mao[idx] = USADA;
        return idx;
    }

    //Backup: caso apresente algum erro inesperado
    //Joga qualquer uma
    for (int i = 0; i < jogador.n_mao; i++) {
        if (!carta_foi_usada(jogador.mao[i])) {
            jogador.mao[i] = USADA;
            return i;
        }
    }

    return 0;
}
