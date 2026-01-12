#include "chip8.h"
#include <emscripten.h>
#include <stdlib.h>
#include <string.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

static Chip8 chip8;

EMSCRIPTEN_KEEPALIVE
void init_chip8(void) {
    chip8_init(&chip8);
}

EMSCRIPTEN_KEEPALIVE
bool load_rom(uint8_t* rom_data, size_t rom_size) {
    return chip8_load_rom(&chip8, rom_data, rom_size);
}

EMSCRIPTEN_KEEPALIVE
void run_cycle(void) {
    chip8_cycle(&chip8);
}

EMSCRIPTEN_KEEPALIVE
void update_timers(void) {
    chip8_update_timers(&chip8);
}

EMSCRIPTEN_KEEPALIVE
uint32_t* get_display(void) {
    return chip8.display;
}

EMSCRIPTEN_KEEPALIVE
bool get_draw_flag(void) {
    bool flag = chip8.draw_flag;
    chip8.draw_flag = false;
    return flag;
}

EMSCRIPTEN_KEEPALIVE
void set_key(uint8_t key, bool pressed) {
    chip8_set_key(&chip8, key, pressed);
}

EMSCRIPTEN_KEEPALIVE
bool get_sound_timer(void) {
    return chip8.sound_timer > 0;
}

EMSCRIPTEN_KEEPALIVE
void copy_display(uint32_t* buffer) {
    memcpy(buffer, chip8.display, DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint32_t));
}

EMSCRIPTEN_KEEPALIVE
uint32_t get_pixel(uint8_t x, uint8_t y) {
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
        return 0;
    }
    return chip8.display[y * DISPLAY_WIDTH + x];
}
