// BIBLIOTECAS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// CONSTANTES
#define VGA_BASE 0xc8000000
#define FPGA_PS2_KEYBOARD 0xFF200100

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
    
#define SNAKE_MAX_SIZE 100

// DECLARAÇÃO DE VARIÁVEIS GLOBAIS
const int max_c = WIDTH / (TILE_SIZE + 1);
const int max_l = HEIGHT / (TILE_SIZE + 1);

uint16_t (*tela)[LWIDTH] = (uint16_t (*)[LWIDTH]) VGA_BASE;

// DECLARAÇÃO DE SPRITES
const uint16_t HEAD_RIGHT[9][9] = {
    {TRANS , TRANS , BLACK , BLACK , BLACK , BLACK , BLACK , TRANS , TRANS },
    {TRANS , BLACK , GREEN , GREEN , BLACK , WHITE , WHITE , BLACK , TRANS },
    {BLACK , GREEN , GREEN , GREEN , GREEN , BLACK , BLACK , GREEN , TRANS },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , RED   , BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , BLACK , BLACK , GREEN , TRANS },
    {TRANS , BLACK , GREEN , GREEN , BLACK , WHITE , WHITE , BLACK , TRANS },
    {TRANS , TRANS , BLACK , BLACK , BLACK , BLACK , BLACK , TRANS , TRANS }
};

const uint16_t HEAD_LEFT[9][9] = {
    {TRANS , TRANS , BLACK , BLACK , BLACK , BLACK , BLACK , TRANS , TRANS },
    {TRANS , BLACK , WHITE , WHITE , BLACK , GREEN , GREEN , BLACK , TRANS },
    {TRANS , GREEN , BLACK , BLACK , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , RED   , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {TRANS , GREEN , BLACK , BLACK , GREEN , GREEN , GREEN , GREEN , BLACK },
    {TRANS , BLACK , WHITE , WHITE , BLACK , GREEN , GREEN , BLACK , TRANS },
    {TRANS , TRANS , BLACK , BLACK , BLACK , BLACK , BLACK , TRANS , TRANS }
};

const uint16_t HEAD_UP[9][9] = {
    {TRANS , TRANS , TRANS , BLACK , BLACK , BLACK , TRANS , TRANS , TRANS },
    {TRANS , BLACK , GREEN , GREEN , RED   , GREEN , GREEN , BLACK , TRANS },
    {BLACK , WHITE , BLACK , GREEN , GREEN , GREEN , BLACK , WHITE , BLACK },
    {BLACK , WHITE , BLACK , GREEN , GREEN , GREEN , BLACK , WHITE , BLACK },
    {BLACK , BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK , BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {TRANS , BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK , TRANS },
    {TRANS , TRANS , BLACK , BLACK , BLACK , BLACK , BLACK , TRANS , TRANS }
};

const uint16_t HEAD_DOWN[9][9] = {
    {TRANS , TRANS , BLACK , BLACK , BLACK , BLACK , BLACK , TRANS , TRANS },
    {TRANS , BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK , TRANS },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK , BLACK },
    {BLACK , WHITE , BLACK , GREEN , GREEN , GREEN , BLACK , WHITE , BLACK },
    {BLACK , WHITE , BLACK , GREEN , GREEN , GREEN , BLACK , WHITE , BLACK },
    {TRANS , BLACK , GREEN , GREEN , RED   , GREEN , GREEN , BLACK , TRANS },
    {TRANS , TRANS , TRANS , BLACK , BLACK , BLACK , TRANS , TRANS , TRANS }
};

const uint16_t (*HEAD[4])[9] = { HEAD_LEFT, HEAD_UP, HEAD_RIGHT, HEAD_DOWN };

const uint16_t TAIL_RIGHT[9][9] = {
    {TRANS , TRANS , TRANS , BLACK , BLACK , BLACK , BLACK , TRANS , TRANS },
    {TRANS , TRANS , BLACK , GREEN , GREEN , GREEN , GREEN , BLACK , TRANS },
    {TRANS , BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , YELLOW, YELLOW, BLACK , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , YELLOW, WHITE , YELLOW, BLACK , GREEN , GREEN , GREEN , BLACK },
    {BLACK , YELLOW, YELLOW, BLACK , GREEN , GREEN , GREEN , GREEN , BLACK },
    {TRANS , BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {TRANS , TRANS , BLACK , GREEN , GREEN , GREEN , GREEN , BLACK , TRANS },
    {TRANS , TRANS , TRANS , BLACK , BLACK , BLACK , BLACK , TRANS , TRANS }
};

const uint16_t TAIL_LEFT[9][9] = {
    {TRANS , TRANS , BLACK , BLACK , BLACK , BLACK , TRANS , TRANS , TRANS },
    {TRANS , BLACK , GREEN , GREEN , GREEN , GREEN , BLACK , TRANS , TRANS },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK , TRANS },
    {BLACK , GREEN , GREEN , GREEN , GREEN , BLACK , YELLOW, YELLOW, BLACK },
    {BLACK , GREEN , GREEN , GREEN , BLACK , YELLOW, WHITE , YELLOW, BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , BLACK , YELLOW, YELLOW, BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK , TRANS },
    {TRANS , BLACK , GREEN , GREEN , GREEN , GREEN , BLACK , TRANS , TRANS },
    {TRANS , TRANS , BLACK , BLACK , BLACK , BLACK , TRANS , TRANS , TRANS }
};

const uint16_t TAIL_UP[9][9] = {
    {TRANS , TRANS , BLACK , BLACK , BLACK , BLACK , BLACK , TRANS , TRANS },
    {TRANS , BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK , TRANS },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , GREEN , GREEN , GREEN , BLACK , GREEN , GREEN , GREEN , BLACK },
    {BLACK , GREEN , GREEN , BLACK , YELLOW, BLACK , GREEN , GREEN , BLACK },
    {TRANS , BLACK , GREEN , YELLOW, WHITE , YELLOW, GREEN , BLACK , TRANS },
    {TRANS , TRANS , BLACK , YELLOW, YELLOW, YELLOW, BLACK , TRANS , TRANS },
    {TRANS , TRANS , TRANS , BLACK , BLACK , BLACK , TRANS , TRANS , TRANS }
};

const uint16_t TAIL_DOWN[9][9] = {
    {TRANS , TRANS , TRANS , BLACK , BLACK , BLACK , TRANS , TRANS , TRANS },
    {TRANS , TRANS , BLACK , YELLOW, YELLOW, YELLOW, BLACK , TRANS , TRANS },
    {TRANS , BLACK , GREEN , YELLOW, WHITE , YELLOW, GREEN , BLACK , TRANS },
    {BLACK , GREEN , GREEN , BLACK , YELLOW, BLACK , GREEN , GREEN , BLACK },
    {BLACK , GREEN , GREEN , GREEN , BLACK , GREEN , GREEN , GREEN , BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK },
    {TRANS , BLACK , GREEN , GREEN , GREEN , GREEN , GREEN , BLACK , TRANS },
    {TRANS , TRANS , BLACK , BLACK , BLACK , BLACK , BLACK , TRANS , TRANS }
};

const uint16_t (*TAIL[4])[9] = { TAIL_LEFT, TAIL_UP, TAIL_RIGHT, TAIL_DOWN };

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

const uint16_t MACA[9][9] = {
    {TRANS, TRANS, BLACK, BLACK, BLACK, GREEN, GREEN, TRANS, TRANS},
    {TRANS, BLACK, RED,   RED,   RED,   GREEN, GREEN, BLACK, TRANS},
    {BLACK, RED,   RED,   RED,   RED,   GREEN, RED,   RED,   BLACK},
    {BLACK, RED,   RED,   RED,   RED,   RED,   RED,   RED,   BLACK},
    {BLACK, RED,   RED,   RED,   RED,   RED,   RED,   RED,   BLACK},
    {BLACK, RED,   RED,   RED,   RED,   RED,   RED,   RED,   BLACK},
    {BLACK, RED,   RED,   RED,   RED,   RED,   RED,   RED,   BLACK},
    {TRANS, BLACK, RED,   RED,   RED,   RED,   RED,   BLACK, TRANS},
    {TRANS, TRANS, BLACK, BLACK, BLACK, BLACK, BLACK, TRANS, TRANS}
};

// FUNÇÕES
void show_pixel(int l, int c, uint16_t cor) {
    if (l >= 0 && l < HEIGHT && c >= 0 && c < WIDTH) {
        if (cor != TRANS) tela[l][c] = cor;
    }
}

void show_tile(int grid_l, int grid_c, const uint16_t tile[TILE_SIZE][TILE_SIZE]) {
    int pixel_l = grid_l * (TILE_SIZE + 1) + 1;
    int pixel_c = grid_c * (TILE_SIZE + 1) + 1;

    for (int l = 0; l < TILE_SIZE; l++) {
        for (int c = 0; c < TILE_SIZE; c++) {
            show_pixel(pixel_l + l, pixel_c + c, tile[l][c]);
        }
    }
}

void clear_tile(int grid_l, int grid_c) {
    int pixel_l = grid_l * (TILE_SIZE + 1) + 1;
    int pixel_c = grid_c * (TILE_SIZE + 1) + 1;

    for (int l = 0; l < TILE_SIZE; l++) {
        for (int c = 0; c < TILE_SIZE; c++) {
            show_pixel(pixel_l + l, pixel_c + c, BG_COLOR);
        }
    }
}

void tela_fundo() {
    for (int l = 0; l < HEIGHT; l++) {
        for (int c = 0; c < WIDTH; c++) {
            if ((l % (TILE_SIZE + 1) == 0) || (c % (TILE_SIZE + 1) == 0)) {
                show_pixel(l, c, GRID_COLOR);
            } else {
                show_pixel(l, c, BG_COLOR);
            }
        }
    }
}

char keyboard_input() {
    volatile int *ps2_pointer = (volatile int *) FPGA_PS2_KEYBOARD;
    static char break_next = 0;
    static char last_valid = 0; 
    char novo_comando = 0;

    int dados_ps2 = *ps2_pointer;
    while (dados_ps2 & 0x8000) {
        unsigned char code = (unsigned char)(dados_ps2 & 0xFF);
        if (break_next) {
            if (code == last_valid) last_valid = 0;
            break_next = 0;
        } else if (code == 0xF0) { // próximo byte é o break code
            break_next = 1;
        } else {
            if (code != last_valid) {
                last_valid = code;
                if (code == 0x74) novo_comando = 'd';
                if (code == 0x6B) novo_comando = 'e';
            }
        }
        dados_ps2 = *ps2_pointer;
    }
    return novo_comando;
}

void delay(int tempo) {
    if (tempo <= 0) return;
    volatile int contador;
    int limite = tempo * 5000; 
    for (contador = 0; contador < limite; contador++) {
        asm("nop");
    }
}

void show_snake(int pos[SNAKE_MAX_SIZE][2], int snake_size, int direction) {
    show_tile(pos[0][0], pos[0][1], HEAD[direction]);
    for (int i = 1; i < (snake_size - 1); i++) show_tile(pos[i][0], pos[i][1], BODY);
    show_tile(pos[snake_size - 1][0], pos[snake_size - 1][1], TAIL[direction]);
}

void gerar_maca(int *maca_l, int *maca_c, int pos[SNAKE_MAX_SIZE][2], int snake_size) {
    int valido = 0;
    while (!valido) {
        valido = 1;
        *maca_l = rand() % max_l;
        *maca_c = rand() % max_c;

        // checa se a maçã não apareceu em cima do corpo da cobra
        for (int i = 0; i < snake_size; i++) {
            if (pos[i][0] == *maca_l && pos[i][1] == *maca_c) {
                valido = 0;
                break;
            }
        }
    }
}

// MAIN
int main() { // loop do jogo
    srand(*(volatile int *) 0xFFFEC600); // usa o timer como seed para geração aleatória de números

    // variáveis de controle
    int pos[SNAKE_MAX_SIZE][2];
    int old_pos[SNAKE_MAX_SIZE][2];
    int snake_size = 5;
    int direction = 2;
    int maca_l, maca_c;

    // posições iniciais da cobra
    pos[0][0] = 12; pos[0][1] = 4;
    pos[1][0] = 12; pos[1][1] = 3;
    pos[2][0] = 12; pos[2][1] = 2;
    pos[3][0] = 12; pos[3][1] = 1;
    pos[4][0] = 12; pos[4][1] = 0;
    
    tela_fundo();
    gerar_maca(&maca_l, &maca_c, pos, snake_size);
    
    int perdeu = 0;
    while (!perdeu) {
        // imprime a cobra na nova posição
        show_snake(pos, snake_size, direction);
        // caso a cauda da cobra tenha passado por cima da maçã, desenha de novo
        show_tile(maca_l, maca_c, MACA);
        
        // lê input do teclado
        char input = keyboard_input();
        delay(20);
        
        // muda a direção da cobra dependendo do input
        switch (input) {
            case 'd':
                direction = (direction + 1) % 4;
                break;
            case 'e':
                direction = (direction + 3) % 4;
                break;
        }
        
        // salva a última posição da cobra
        for (int i = 0; i < snake_size; i++) {
            old_pos[i][0] = pos[i][0];
            old_pos[i][1] = pos[i][1];
        }

        // calcula a nova posição da cabeça
        switch (direction) {
            case 0:
                pos[0][1] = (pos[0][1] - 1 + max_c) % max_c;
                break;
            case 1:
                pos[0][0] = (pos[0][0] - 1 + max_l) % max_l;
                break;
            case 2:
                pos[0][1] = (pos[0][1] + 1) % max_c;
                break;
            case 3:
                pos[0][0] = (pos[0][0] + 1) % max_l;
                break;
        }

        // calcula a nova posição do do resto do corpo
        for (int i = 1; i < snake_size; i++) {
            pos[i][0] = old_pos[i - 1][0];
            pos[i][1] = old_pos[i - 1][1];
        }

        // apaga a ponta da cauda antiga
        clear_tile(old_pos[snake_size - 1][0], old_pos[snake_size - 1][1]);

        // checa se a cabeça colidiu com o corpo
        for (int i = 1; i < snake_size; i++) {
            if (pos[0][0] == pos[i][0] && pos[0][1] == pos[i][1]) perdeu = 1;
        }

        // checa se a cobra comeu a maçã
        if (pos[0][0] == maca_l && pos[0][1] == maca_c) {
            // inicializa nova posição da cauda
            pos[snake_size][0] = old_pos[snake_size - 1][0];
            pos[snake_size][1] = old_pos[snake_size - 1][1];
            snake_size++;
            gerar_maca(&maca_l, &maca_c, pos, snake_size);
        }
    }
    return 0;
}
