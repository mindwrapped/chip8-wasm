#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MEMORY_SIZE 4096
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define STACK_SIZE 16
#define NUM_REGISTERS 16
#define NUM_KEYS 16

typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint8_t V[NUM_REGISTERS];  // General purpose registers V0-VF
    uint16_t I;                // Index register
    uint16_t PC;               // Program counter
    uint16_t stack[STACK_SIZE];
    uint8_t SP;                // Stack pointer
    uint8_t delay_timer;
    uint8_t sound_timer;
    
    uint32_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];  // Display buffer (RGBA)
    bool keys[NUM_KEYS];       // Keyboard state
    
    bool draw_flag;            // Flag to indicate display needs redraw
} Chip8;

// Initialize the CHIP-8 system
void chip8_init(Chip8* chip8);

// Load a ROM into memory
bool chip8_load_rom(Chip8* chip8, const uint8_t* rom, size_t rom_size);

// Execute one instruction
void chip8_cycle(Chip8* chip8);

// Decrement timers (call at 60Hz)
void chip8_update_timers(Chip8* chip8);

// Set key state
void chip8_set_key(Chip8* chip8, uint8_t key, bool pressed);

// Get key state
bool chip8_get_key(Chip8* chip8, uint8_t key);

#endif // CHIP8_H
