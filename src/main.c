#include "chip8.h"
#include <SDL2/SDL.h>
#include <unistd.h>

#define SCALE 15
#define AMPLITUDE 28000
#define SAMPLE_RATE 44100

void audio_callback(void *user_data, uint8_t *raw_buffer, int len) {
  int16_t *buffer = (int16_t *)raw_buffer;
  int length = len / 2; // Because we're using 16-bit audio
  static uint32_t running_sample_index = 0;

  // The CHIP-8 "Beep" frequency (around 440Hz)
  int tone_hz = 440;
  int period = SAMPLE_RATE / tone_hz;
  int half_period = period / 2;

  for (int i = 0; i < length; i++) {
    // Generate a square wave: if index % period < half, high; else, low
    buffer[i] = ((running_sample_index++ / half_period) % 2) ? AMPLITUDE : -AMPLITUDE;
  }
}

void handle_input(Chip8 *c8, SDL_Event *event) {
  if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
    uint8_t val = (event->type == SDL_KEYDOWN) ? 1 : 0;

    switch (event->key.keysym.sym) {
    case SDLK_x:
      c8->keypad[0] = val;
      break;
    case SDLK_1:
      c8->keypad[1] = val;
      break;
    case SDLK_2:
      c8->keypad[2] = val;
      break;
    case SDLK_3:
      c8->keypad[3] = val;
      break;
    case SDLK_q:
      c8->keypad[4] = val;
      break;
    case SDLK_w:
      c8->keypad[5] = val;
      break;
    case SDLK_e:
      c8->keypad[6] = val;
      break;
    case SDLK_a:
      c8->keypad[7] = val;
      break;
    case SDLK_s:
      c8->keypad[8] = val;
      break;
    case SDLK_d:
      c8->keypad[9] = val;
      break;
    case SDLK_z:
      c8->keypad[10] = val;
      break;
    case SDLK_c:
      c8->keypad[11] = val;
      break;
    case SDLK_4:
      c8->keypad[12] = val;
      break;
    case SDLK_r:
      c8->keypad[13] = val;
      break;
    case SDLK_f:
      c8->keypad[14] = val;
      break;
    case SDLK_v:
      c8->keypad[15] = val;
      break;
    }
  }
}

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

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
  }
  SDL_Window *window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * SCALE,
                                        32 * SCALE, SDL_WINDOW_SHOWN);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

  SDL_AudioSpec want, have;
  SDL_zero(want);
  want.freq = SAMPLE_RATE;
  want.format = AUDIO_S16SYS; // 16-bit signed audio
  want.channels = 1;          // Mono
  want.samples = 2048;
  want.callback = audio_callback;

  SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
  if (dev == 0) {
    printf("Failed to open audio: %s\n", SDL_GetError());
  }

  uint32_t pixels[RESOLUTION];
  SDL_Event event;

  uint8_t instructions_per_frame = 10;
  int running = 1;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        running = 0;
      handle_input(&c8, &event);
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

    if (c8.sound_counter > 0) {
      SDL_PauseAudioDevice(dev, 0); // 0 means "unpause" (play)
    } else {
      SDL_PauseAudioDevice(dev, 1); // 1 means "pause" (silent)
    }
    SDL_Delay(16);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_CloseAudioDevice(dev);
  SDL_Quit();
  return 0;
}
