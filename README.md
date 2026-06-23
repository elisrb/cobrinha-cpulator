# Jogo da Cobrinha no CPUlator

O trabalho consiste em desenvolver uma réplica do clássico jogo da cobrinha em C, que rode no [simulador ARMv7 (De1-SoC) no CPUlator](https://cpulator.01xz.net/?sys=arm-de1soc).

## Funcionamento
A cobra se desloca em direções ortogonais, virando 90 graus à esquerda ou direita sob o comando das teclas. Maçãs vermelhas surgem na tela (uma de cada vez) em posições aleatórias. Quando a cabeça da cobra encontra uma maçã (come a maçã):

- a maçã desaparece
- a cobra aumenta uma unidade em seu comprimento
- outra maçã é gerada aleatoriamente

Se a cobra atingir o seu corpo, ela morre e encerra o jogo. Se atingir o limite da tela, ela rebate para o lado oposto.

## Como jogar
1. Cole o código em `cobrinha.c` no [simulador ARMv7 (De1-SoC) no CPUlator](https://cpulator.01xz.net/?sys=arm-de1soc)
2. Clique em `Compile and Load (F5)` e, em seguida, em `Continue (F3)`
4. Use as setas para virar a cobrinha para esquerda ou direita
