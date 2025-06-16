#ifndef JOGADOR_17151550_H
#define JOGADOR_17151550_H

#include "../carta.h"

void nova_rodada_17151550(const int rodada, const Carta carta_virada, const int n_cartas, Carta* minha_mao);
int jogar_17151550(const Carta* mesa, const int num_na_mesa, const int vitorias);
const char* nome_17151550(void);
void iniciar_17151550(const int id, const int total_jogadores);
int apostar_17151550(const int* apostas);

#endif // JOGADOR_17151550_H
