// BIBLIOTECAS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CONSTANTES
#define VGA_BASE 0xc8000000
#define FPGA_PS2_KEYBOARD 0xFF200100
#define MPCORE_PRIV_TIMER_LOAD 0xFF202000
#define MPCORE_PRIV_TIMER_CONTROL 0xFF202008
#define MPCORE_PRIV_TIMER_STATUS 0xFF20200C

#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define BLACK 0x0000
#define WHITE 0xFFFF
#define GRAY 0x8410
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define CYAN 0x87FF
#define TRANS 0x8001

#define WIDTH 320
#define LWIDTH 512
#define HEIGHT 240

#define TILE_SIZE 9
#define BG_COLOR BLACK
#define GRID_COLOR GRAY

// DECLARAÇÃO DE VARIÁVEIS GLOBAIS
int head_pos[2] = {12, 2}; // posição inicial da cabeça
int direction = 2; // direção inicial (direita)
/*	DIREÇÕES
	0 - Esquerda
	1 - Cima			1
	2 - Direita		0		2
	3 - Baixo			3
*/

const int max_c = WIDTH/(TILE_SIZE+1);
const int max_l = HEIGHT/(TILE_SIZE+1);

uint16_t (*tela)[LWIDTH] = (uint16_t (*)[LWIDTH]) VGA_BASE;

const uint16_t HEAD_RIGHT[9][9] = {
    {TRANS, TRANS, BLACK, BLACK, BLACK, BLACK, BLACK, TRANS, TRANS},
    {TRANS, BLACK, GREEN, GREEN, BLACK, WHITE, WHITE, BLACK, TRANS},
    {BLACK, GREEN, GREEN, GREEN, GREEN, BLACK, BLACK, GREEN, TRANS},
    {BLACK, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, BLACK},
    {BLACK, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, RED,   BLACK},
    {BLACK, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, BLACK},
    {BLACK, GREEN, GREEN, GREEN, GREEN, BLACK, BLACK, GREEN, TRANS},
    {TRANS, BLACK, GREEN, GREEN, BLACK, WHITE, WHITE, BLACK, TRANS},
    {TRANS, TRANS, BLACK, BLACK, BLACK, BLACK, BLACK, TRANS, TRANS}
};

const uint16_t BODY[9][9] = {
    {TRANS, TRANS, BLACK, BLACK, BLACK, BLACK, BLACK, TRANS, TRANS},
    {TRANS, BLACK, GREEN, GREEN, GREEN, GREEN, GREEN, BLACK, TRANS},
    {BLACK, GREEN, GREEN, GREEN, CYAN,  GREEN, GREEN, GREEN, BLACK},
    {BLACK, GREEN, GREEN, CYAN,  CYAN,  CYAN,  GREEN, GREEN, BLACK},
    {BLACK, GREEN, CYAN,  CYAN,  GREEN, CYAN,  CYAN,  GREEN, BLACK},
    {BLACK, GREEN, GREEN, CYAN,  CYAN,  CYAN,  GREEN, GREEN, BLACK},
    {BLACK, GREEN, GREEN, GREEN, CYAN,  GREEN, GREEN, GREEN, BLACK},
    {TRANS, BLACK, GREEN, GREEN, GREEN, GREEN, GREEN, BLACK, TRANS},
    {TRANS, TRANS, BLACK, BLACK, BLACK, BLACK, BLACK, TRANS, TRANS}
};

const uint16_t TAIL_RIGHT[9][9] = {
    {TRANS, TRANS,  TRANS,  BLACK,  BLACK, BLACK, BLACK, TRANS, TRANS},
    {TRANS, TRANS,  BLACK,  GREEN,  GREEN, GREEN, GREEN, BLACK, TRANS},
    {TRANS, BLACK,  GREEN,  GREEN,  GREEN, GREEN, GREEN, GREEN, BLACK},
    {BLACK, YELLOW, YELLOW, BLACK,  GREEN, GREEN, GREEN, GREEN, BLACK},
    {BLACK, YELLOW, WHITE,  YELLOW, BLACK, GREEN, GREEN, GREEN, BLACK},
    {BLACK, YELLOW, YELLOW, BLACK,  GREEN, GREEN, GREEN, GREEN, BLACK},
    {TRANS, BLACK,  GREEN,  GREEN,  GREEN, GREEN, GREEN, GREEN, BLACK},
    {TRANS, TRANS,  BLACK,  GREEN,  GREEN, GREEN, GREEN, BLACK, TRANS},
    {TRANS, TRANS,  TRANS,  BLACK,  BLACK, BLACK, BLACK, TRANS, TRANS}
};

// FUNÇÕES
void show_pixel(int l, int c, uint16_t cor) {
    if (l >= 0 && l < HEIGHT && c >= 0 && c < WIDTH) {
        if (cor != TRANS) {
            tela[l][c] = cor;
        }
    }
}

void show_tile(int grid_l, int grid_c, uint16_t tile[TILE_SIZE][TILE_SIZE]) {
    int pixel_l = grid_l * (TILE_SIZE+1) + 1;
    int pixel_c = grid_c * (TILE_SIZE+1) + 1;

    for (int l = 0; l < TILE_SIZE; l++) {
        for (int c = 0; c < TILE_SIZE; c++) {
            show_pixel(pixel_l+l, pixel_c+c, tile[l][c]);
        }
    }
}

void clear_tile(int grid_l, int grid_c) {
    int pixel_l = grid_l * (TILE_SIZE+1) + 1;
    int pixel_c = grid_c * (TILE_SIZE+1) + 1;

    for (int l = 0; l < TILE_SIZE; l++) {
        for (int c = 0; c < TILE_SIZE; c++) {
            show_pixel(pixel_l+l, pixel_c+c, BG_COLOR);
        }
    }
}

void tela_fundo() {
    uint16_t cor_pixel;
	for (int l=0; l < HEIGHT; l++) {
		for (int c=0; c < WIDTH; c++) {
			if((l%(TILE_SIZE+1) == 0) || (c%(TILE_SIZE+1) == 0)){
				cor_pixel = GRID_COLOR;
			} else {
				cor_pixel = BG_COLOR;
			}
			show_pixel(l, c, cor_pixel);
		}
	}
}

char keyboard_input() {
    volatile int *ps2_pointer = (volatile int *) FPGA_PS2_KEYBOARD;
    int dados_ps2 = *ps2_pointer;
    
    // Lê o registrador até esvaziar o buffer FIFO do teclado de cliques antigos repetidos
    char ultimo_comando = 0;
    while (dados_ps2 & 0x8000) {
        unsigned char code = (unsigned char)(dados_ps2 & 0xFF);
        if (code == 0x74) ultimo_comando = 'd';
        if (code == 0x6B) ultimo_comando = 'e';
        dados_ps2 = *ps2_pointer;
    }
    return ultimo_comando;
}

void delay(int tempo) {
    if (tempo <= 0) return;
    volatile int contador;
    int limite = tempo * 50000; 
	for (contador = 0; contador < limite; contador++) {
        asm("nop");
    }
}

// MAIN
int main() {
	tela_fundo();
	int old_pos[2] = {0, 0};
	
    while (1) {
		show_tile(head_pos[0], head_pos[1], HEAD_RIGHT);
		clear_tile(old_pos[0], old_pos[1]);
		
        char input = 0;
		for (int i = 0; i < 100; i++) {
            char clique = keyboard_input();
            if (clique != 0) {
                input = clique; // Salva o último clique válido feito nessa janela de tempo
            }
            delay(1); // Um delay curtíssimo apenas para cadenciar o loop de leitura
        }
		
        switch (input) {
            case 'd':
				direction = (direction + 1) % 4;
                break;
            case 'e':
				direction = (direction + 3) % 4;
                break;
        }
		
		old_pos[0] = head_pos[0];
		old_pos[1] = head_pos[1];
		
		switch (direction) {
			case 0:
				head_pos[1] = (head_pos[1] - 1 + max_c) % max_c;
				break;
			case 1:
				head_pos[0] = (head_pos[0] - 1 + max_l) % max_l;
				break;
			case 2:
				head_pos[1] = (head_pos[1] + 1) % max_c;
				break;
			case 3:
				head_pos[0] = (head_pos[0] + 1) % max_l;
				break;
		}
		
		delay(5);
    }

	return 0;
}
