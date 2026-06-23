// BIBLIOTECAS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CONSTANTES
#define VGA_BASE 0xc8000000
#define FPGA_CHAR_BASE 0xC9000000
#define FPGA_PS2_KEYBOARD 0xFF200100
#define MPCORE_PRIV_TIMER_LOAD 0xFF202000
#define MPCORE_PRIV_TIMER_VALUE 0xFF202004
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
#define CYAN 0x07FF
#define TRANS 0x8001

#define WIDTH 320
#define LWIDTH 512
#define HEIGHT 240

#define TILE_SIZE 8
#define SNAKE_MAX_SIZE  (WIDTH/TILE_SIZE)*(HEIGHT/TILE_SIZE)
	
// DECLARAÇÃO DE FUNÇÕES	
void tela_fundo();

// DECLARAÇÃO DE VARIÁVEIS GLOBAIS
uint16_t (*tela)[LWIDTH] = (uint16_t (*)[LWIDTH]) VGA_BASE;
const uint16_t HEAD_RIGHT[8][8] = {
 {BLACK, BLACK, BLACK, GREEN, GREEN, BLACK, BLACK, BLACK},
 {BLACK, BLACK, WHITE, GREEN, GREEN, WHITE, BLACK, BLACK},
 {BLACK, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, BLACK},
 {BLACK, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, BLACK},
 {BLACK, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, BLACK},
 {BLACK, BLACK, WHITE, GREEN, GREEN, WHITE, BLACK, BLACK},
 {BLACK, BLACK, GREEN, GREEN, GREEN, GREEN, BLACK, BLACK},
 {BLACK, BLACK, BLACK, TRANS, TRANS, BLACK, BLACK, BLACK},
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
	uint16_t cor = 0;
	for (int l=0; l < HEIGHT; l++) {
		for (int c=0; c < WIDTH; c++) {
			if((l%(TILE_SIZE+1) == 0) || (c%(TILE_SIZE+1) == 0)){
				cor = WHITE;
			} else {
				cor = RED;
			}
			show_pixel(l, c, cor);
		}
	}
}

// MAIN
int main() {
	tela_fundo();
    show_tile(1,1,HEAD_RIGHT);
	return 0;
}