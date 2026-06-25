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

// DECLARAÇÃO DE VARIÁVEIS GLOBAIS
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

void tela_fundo() {
    uint16_t cor_pixel;
	for (int l=0; l < HEIGHT; l++) {
		for (int c=0; c < WIDTH; c++) {
			if((l%(TILE_SIZE+1) == 0) || (c%(TILE_SIZE+1) == 0)){
				cor_pixel = GRAY;
			} else {
				cor_pixel = BLACK;
			}
			show_pixel(l, c, cor_pixel);
		}
	}
}

char keyboard_input() {
    volatile int *ps2_pointer = (volatile int *) FPGA_PS2_KEYBOARD;
    int dados_ps2 = *ps2_pointer;
    if (dados_ps2 & 0x8000) {
		unsigned char code = (unsigned char)(dados_ps2 & 0xFF);
		printf("scan code: 0x%02X\n", code);
        switch (code) {
            case 0x74: // → Direita
				return 'd';
				break;
            case 0x6B: // ← Esquerda
				return 'e';
				break;
            default:
				return 0;
        }
    }
    return 0;
}

void delay(int milisegundos) { // !!! TESTE PENDENTE
    volatile int *timer_load    = (volatile int *) MPCORE_PRIV_TIMER_LOAD;
    volatile int *timer_control = (volatile int *) MPCORE_PRIV_TIMER_CONTROL;
    volatile int *timer_status  = (volatile int *) MPCORE_PRIV_TIMER_STATUS;

    if (milisegundos <= 0) return;

    int ciclos_totais = milisegundos * 100000;
    *timer_load    = ciclos_totais;
    *timer_status  = 1;
    *timer_control = 0x1;

    while ((*timer_status & 0x1) == 0) {
        // aguarda expiração
    }

    *timer_control = 0x0;
}

// MAIN
int main() {
	tela_fundo();
    show_tile(12,2,HEAD_RIGHT);
    show_tile(12,1,BODY);
    show_tile(12,0,TAIL_RIGHT);

    int head_pos = 2;

    while (1) {
        char input = keyboard_input();
        switch (input) {
            case 'd':
                head_pos += 1;
                break;
            case 'e':
                head_pos -= 1;
                break;
        }
		int max_cols = WIDTH/(TILE_SIZE+1);
		head_pos = (head_pos + max_cols) % max_cols;
		show_tile(12,head_pos,HEAD_RIGHT);
    }

	return 0;
}
