#include "chip8.h"
#include <unistd.h>

int main() {
  Chip8 c8;
  chip8_init(&c8);
  uint8_t instructions_per_frame = 10;

  while (1) {
    for (int i = 0; i < instructions_per_frame; i++) {
      chip8_execute(&c8);
    }

    chip8_render(&c8);
    usleep(16666);
    c8.delay_counter > 0 ? c8.delay_counter-- : 0;
    c8.sound_counter > 0 ? c8.sound_counter-- : 0;
  }
  return 0;
}
