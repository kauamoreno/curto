import subprocess
import re #Expressões Regulares
from collections import defaultdict

# Mostra tudo que o ./main escreveu no terminal
def debug(saida):
    print("\n=== Saída completa do ./main ===")
    print(saida)

def verificaErros(erro, returncode):
    if erro:
        print("\n=== STDERR (mensagens de erro) ===")
        print(erro)
    else:
        print("\n[Sem mensagens de erro no stderr]\n")

    if returncode != 0:
        print(f"[!] Erro na execução do programa (código {returncode}):\n{erro}")
        return True
    
    return False

def executar_simulacao(numero_partidas, max_enters=200):
    vencedores = defaultdict(int)
    todos_jogadores = set()
    empates = 0

    for partida in range(1, numero_partidas + 1):
        try:
            processo = subprocess.Popen(
                ['./main'],
                stdout=subprocess.PIPE,
                stdin=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )

            # Envia ENTERs para ate as pontuações finais
            input_enters = '\n' * max_enters
            saida, erro = processo.communicate(input=input_enters) 

            #Descomentar caso queira a saida
            #debug(saida)

            #Verifica erro do console ./main e de execução
            if verificaErros(erro, processo.returncode):
                continue

            # Captura pontuações finais
            pontuacoes_finais = {}
            for linha in saida.splitlines():

                #João da Silva: 42 - Funciona
                #"João da Silva - 42" (usa hífen em vez de dois-pontos) - Não funciona
                if re.match(r'^[\wÀ-ÿ\s]+:\s*\d+$', linha):  # Suporta acentos e espaços
                    nome, pontos = linha.rsplit(':', 1)
                    nome = nome.strip()
                    pontos = pontos.strip()
                    if pontos.isdigit():
                        pontuacoes_finais[nome] = int(pontos)
                        todos_jogadores.add(nome)

            if not pontuacoes_finais:
                print(f"[!] Partida {partida}: Nenhum resultado encontrado.")
                print("Saída do jogo (últimas linhas):")
                print("\n".join(saida.strip().splitlines()[-4:]))  # Mostra últimas 4 linhas
                continue

            menor_pontuacao = min(pontuacoes_finais.values())
            vencedores_partida = [nome for nome, pts in pontuacoes_finais.items() if pts == menor_pontuacao]

            if len(vencedores_partida) == 1:
                vencedores[vencedores_partida[0]] += 1
                status_vencedor = f"🏆 Vencedor: {vencedores_partida[0]} com {menor_pontuacao} pontos"
            else:
                empates += 1
                status_vencedor = f"⚠️ Empate entre: {', '.join(vencedores_partida)} com {menor_pontuacao} pontos (nenhum ponto contabilizado)"

            # Impressão da partida
            print(f"\nPartida {partida}:")
            for jogador, pontos in sorted(pontuacoes_finais.items(), key=lambda x: x[1]):
                print(f"  {jogador}: {pontos} pontos")
            print(f"  {status_vencedor}")


        except FileNotFoundError:
            print("[ERRO] Arquivo './main' não encontrado. Compile ou mova o executável.")
            return
        except Exception as erro:
            print(f"[!] Erro inesperado na partida {partida}: {erro}")
            continue

    # Garante que todos aparecem, mesmo com 0 vitórias
    for jogador in todos_jogadores:
        vencedores.setdefault(jogador, 0)

    # Resultados finais
    print("\n============= 🧮 RANKING FINAL =============")
    for jogador, vitorias in sorted(vencedores.items(), key=lambda x: x[1], reverse=True):
        print(f"{jogador}: {vitorias} vitória{'s' if vitorias != 1 else ''}")
    print("\n============================================")

    if vencedores:
        max_vitorias = max(vencedores.values())
        campeoes = [nome for nome, v in vencedores.items() if v == max_vitorias]
        print(f"\n🎉 CAMPEÃO GERAL: {', '.join(campeoes)} com {max_vitorias} vitória{'s' if max_vitorias != 1 else ''}!")

    print(f"\n🤝 TOTAL DE EMPATES: {empates}")
    print("\n============================================")


if __name__ == "__main__":
    try:
        numero = 100
        #numero = int(input("Quantas partidas deseja simular? "))
        
        executar_simulacao(numero)
    except ValueError:
        print("Por favor, digite um número válido.")
