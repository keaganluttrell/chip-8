#include "chip8.h"
#include <SDL2/SDL.h>
#include <unistd.h>

#define SCALE 15

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s <rom_path>\n", argv[0]);
    return 1;
  }

  Chip8 c8;
  chip8_init(&c8);

  if (chip8_load_rom(&c8, argv[1]) != FN_SUCCESS) {
    return 1;
  }

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * SCALE,
                                        32 * SCALE, SDL_WINDOW_SHOWN);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

  uint32_t pixels[RESOLUTION];
  SDL_Event event;

  uint8_t instructions_per_frame = 10;
  int running = 1;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        running = 0;
      // Handle keydown/keyup for c8.keypad here...
    }

    for (int i = 0; i < instructions_per_frame; i++) {
      chip8_execute(&c8);
    }

    chip8_step_timers(&c8);
    if (c8.draw_flag) {
      for (int i = 0; i < RESOLUTION; i++) {
        pixels[i] = (c8.display[i]) ? 0xFFFFFFFF : 0x000000FF; // White or Black
      }

      SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
      c8.draw_flag = 0;
    }

    SDL_Delay(16);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
