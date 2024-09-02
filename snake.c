#include "snake.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_X SDL_WINDOWPOS_CENTERED
#define WINDOW_Y SDL_WINDOWPOS_CENTERED
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

#define GRID_SIZE 30
#define GRID_DIM 800

#define HIDDEN_GRID 0

enum {
  SNAKE_UP,
  SNAKE_DOWN,
  SNAKE_LEFT,
  SNAKE_RIGHT,
};

typedef struct {
  int x;
  int y;
} apple;

apple Apple;

struct snake {
  int x;
  int y;
  int dir;
  struct snake *next;
};
typedef struct snake Snake;

Snake *head;
Snake *tail;

void init_snake() {

  Snake *new = malloc(sizeof(Snake));
  new->x = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
  new->y = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
  new->dir = SNAKE_UP;
  new->next = NULL;

  head = new;
  tail = new;

  return;
}

void move_snake() {

  int prev_x = head->x; // set as com a posicao atual da cabeca
  int prev_y = head->y;
  int prev_dir = head->dir;

  switch (head->dir) {
  case SNAKE_UP:
    head->y--;
    break;
  case SNAKE_DOWN:
    head->y++;
    break;
  case SNAKE_LEFT:
    head->x--;
    break;
  case SNAKE_RIGHT:
    head->x++;
    break;
  }

  Snake *track = head;

  if (track->next != NULL) { // tem que ser diferente de NULL pois se for igual
                             // ele nao vai ter proximo na lista
    track = track->next;     // Joga para o proximo rect no corpo
  }

  while (track != NULL) {

    int save_x = track->x; // salva o valor do bloco atual pois vai ser usado no
                           // proximo para ficar no lugar dele
    int save_y = track->y;
    int save_dir = track->dir;

    track->x = prev_x; // seta as variaveis do track com o valor atual da
    track->y = prev_y;
    track->dir = prev_dir;

    track = track->next; // jogar para o proximo

    prev_x = save_x; // set a posicao, funciona como se fosse a cabeca, pois pra
                     // cada um que esta atras
    prev_y = save_y; // os da frente sao os cabecas
    prev_dir = save_dir;
  }

  return;
}

void increment_snake() {

  Snake *new = malloc(sizeof(Snake));

  switch (tail->dir) {
  case SNAKE_UP:
    new->x = tail->x;
    new->y = tail->y + 1;
    break;
  case SNAKE_DOWN:
    new->x = tail->x;
    new->y = tail->y - 1;
    break;
  case SNAKE_LEFT:
    new->x = tail->x + 1;
    new->y = tail->y;
    break;
  case SNAKE_RIGHT:
    new->x = tail->x - 1;
    new->y = tail->y;
    break;
  }

  new->dir = tail->dir;

  new->next = NULL;
  tail->next = new;

  tail = new;

  return;
}

void reset_snake() {

  Snake *track = head;

  Snake *temp;

  while (track != NULL) {
    temp = track;
    track = track->next;
    free(temp);
  }

  init_snake();
  increment_snake();
  increment_snake();
  increment_snake();

  return;
}

void render_snake(SDL_Renderer *renderer, int x, int y) {

  SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 255);

  SDL_Rect seg;
  int seg_size = GRID_DIM / GRID_SIZE;
  seg.w = seg_size;
  seg.y = seg_size;

  Snake *track = head;

  while (track != NULL) {
    seg.x = x + track->x * seg_size;
    seg.y = y + track->y * seg_size;
    SDL_RenderFillRect(renderer, &seg);

    track = track->next;
  }

  return;
}

void render_grid(SDL_Renderer *renderer, int x, int y) {

  SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 255);

#if 1
  int cell_size = GRID_DIM / GRID_SIZE;

  SDL_Rect cell;

  cell.w = cell_size;
  cell.h = cell_size;

  for (int i = 0; i < GRID_SIZE; i++) {
    for (int j = 0; j < GRID_SIZE; j++) {
      cell.x = x + (i * cell_size);
      cell.y = y + (j * cell_size);
      SDL_RenderDrawRect(renderer, &cell);
    }
  }
#else

  SDL_Rect outline;
  outline.x = x;
  outline.y = y;
  outline.w = GRID_DIM;
  outline.h = GRID_DIM;

  SDL_RenderDrawRect(renderer, &outline);

#endif

  return;
}

void gen_apple() {

  bool in_snake;

  do {
    in_snake = false;
    Apple.x = rand() % GRID_SIZE;
    Apple.y = rand() % GRID_SIZE;

    Snake *track = head;

    while (track != NULL) {

      if (track->x == Apple.x && track->y == Apple.y) {
        in_snake = true;
      }

      track = track->next;
    }
  } while (in_snake);

  return;
}

void render_apple(SDL_Renderer *renderer, int x, int y) {

  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 255);

  int apple_size = GRID_DIM / GRID_SIZE;

  SDL_Rect app;

  app.w = apple_size;
  app.h = apple_size;
  app.x = x + Apple.x * apple_size;
  app.y = y + Apple.y * apple_size;

  SDL_RenderFillRect(renderer, &app);

  return;
}

void detect_apple() {
  if (head->x == Apple.x && head->y == Apple.y) {
    gen_apple();
    increment_snake();
  }
}

void detect_crash() {

  if (head->x < 0 || head->x >= GRID_SIZE || head->y < 0 ||
      head->y >= GRID_SIZE) {
    reset_snake();
  }

  Snake *track = head;

  if (track->next != NULL) {
    track = track->next;
  }

  while (track != NULL) {

    if (track->x == head->x && track->y == head->y) {
      reset_snake();
    }

    track = track->next;
  }

  return;
}

int main() {

  srand(time(0));

  init_snake();
  increment_snake();
  increment_snake();
  increment_snake();
  increment_snake();

  gen_apple();

  SDL_Window *window;     // Cria uma janela
  SDL_Renderer *renderer; // rederizador

  if (SDL_INIT_VIDEO < 0) {
    fprintf(stderr, "ERROR: SDL_INIT_VIDEO"); // Verifica se a iniciacializacao
                                              // foi com sucesso
  }
  window = SDL_CreateWindow("Snake", WINDOW_X, WINDOW_Y,
                            WINDOW_WIDTH, // Cria a janela nos tamanhos
                            WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS);

  if (!window) {
    fprintf(stderr, "ERROR: !window"); // Verifica se a janela foi criada
  }

  renderer = SDL_CreateRenderer(
      window, -1,
      SDL_RENDERER_ACCELERATED); // Cria um render com acelerador de hardware

  if (!renderer) {
    fprintf(stderr, "ERROR: !renderer"); // Faz a mesma vericacao
  }

  int grid_x = (WINDOW_WIDTH / 2) - (GRID_DIM / 2);
  int grid_y = (WINDOW_HEIGHT / 2) - (GRID_DIM / 2);

  bool quit = false;
  SDL_Event event;

  while (!quit) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_KEYUP:
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          quit = true;
          break;
        case SDLK_UP:
          head->dir = SNAKE_UP;
          break;
        case SDLK_DOWN:
          head->dir = SNAKE_DOWN;
          break;
        case SDLK_LEFT:
          head->dir = SNAKE_LEFT;
          break;
        case SDLK_RIGHT:
          head->dir = SNAKE_RIGHT;
          break;
        }
        break;
      }
    }
    SDL_RenderClear(renderer); // Limpa o render
    // RENDER LOOP START

    move_snake();
    detect_apple();
    detect_crash();

    render_grid(renderer, grid_x, grid_y);
    render_snake(renderer, grid_x, grid_y);
    render_apple(renderer, grid_x, grid_y);

    // RENDER LOOP END
    SDL_SetRenderDrawColor(renderer, 0x11, 0x11, 0x11,
                           255); // Redenriza uma cor de fundo
    SDL_RenderPresent(renderer); //
    SDL_Delay(80);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
