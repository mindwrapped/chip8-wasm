#include "chip8.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Font data (4x5 pixels per character, stored as 5 bytes each)
static const uint8_t font_data[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(Chip8* chip8) {
    memset(chip8, 0, sizeof(Chip8));
    
    // Load font into memory (0x000-0x050)
    memcpy(chip8->memory, font_data, sizeof(font_data));
    
    // Program counter starts at 0x200
    chip8->PC = 0x200;
    
    // Initialize display to black
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        chip8->display[i] = 0x00000000; // Black (RGBA)
    }
}

bool chip8_load_rom(Chip8* chip8, const uint8_t* rom, size_t rom_size) {
    if (rom_size > MEMORY_SIZE - 0x200) {
        return false; // ROM too large
    }
    
    memcpy(&chip8->memory[0x200], rom, rom_size);
    chip8->PC = 0x200;
    return true;
}

void chip8_update_timers(Chip8* chip8) {
    if (chip8->delay_timer > 0) {
        chip8->delay_timer--;
    }
    if (chip8->sound_timer > 0) {
        chip8->sound_timer--;
    }
}

void chip8_set_key(Chip8* chip8, uint8_t key, bool pressed) {
    if (key < NUM_KEYS) {
        chip8->keys[key] = pressed;
    }
}

bool chip8_get_key(Chip8* chip8, uint8_t key) {
    if (key < NUM_KEYS) {
        return chip8->keys[key];
    }
    return false;
}

static uint8_t random_byte(void) {
    return rand() & 0xFF;
}

void chip8_cycle(Chip8* chip8) {
    // Fetch instruction (2 bytes, big-endian)
    uint16_t opcode = (chip8->memory[chip8->PC] << 8) | chip8->memory[chip8->PC + 1];
    
    // Extract instruction components
    uint16_t nnn = opcode & 0x0FFF;  // Address
    uint8_t n = opcode & 0x000F;     // 4-bit number
    uint8_t x = (opcode >> 8) & 0x0F; // Register X
    uint8_t y = (opcode >> 4) & 0x0F; // Register Y
    uint8_t kk = opcode & 0x00FF;    // 8-bit constant
    
    chip8->PC += 2; // Increment PC
    
    // Decode and execute instruction
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0: // CLS - Clear display
                    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
                        chip8->display[i] = 0x00000000;
                    }
                    chip8->draw_flag = true;
                    break;
                    
                case 0x00EE: // RET - Return from subroutine
                    chip8->SP--;
                    chip8->PC = chip8->stack[chip8->SP];
                    break;
                    
                default:
                    // 0NNN - SYS addr (ignored in modern interpreters)
                    break;
            }
            break;
            
        case 0x1000: // JP addr - Jump to address
            chip8->PC = nnn;
            break;
            
        case 0x2000: // CALL addr - Call subroutine
            chip8->stack[chip8->SP] = chip8->PC;
            chip8->SP++;
            chip8->PC = nnn;
            break;
            
        case 0x3000: // SE Vx, byte - Skip if Vx == kk
            if (chip8->V[x] == kk) {
                chip8->PC += 2;
            }
            break;
            
        case 0x4000: // SNE Vx, byte - Skip if Vx != kk
            if (chip8->V[x] != kk) {
                chip8->PC += 2;
            }
            break;
            
        case 0x5000: // SE Vx, Vy - Skip if Vx == Vy
            if (chip8->V[x] == chip8->V[y]) {
                chip8->PC += 2;
            }
            break;
            
        case 0x6000: // LD Vx, byte - Set Vx = kk
            chip8->V[x] = kk;
            break;
            
        case 0x7000: // ADD Vx, byte - Set Vx = Vx + kk
            chip8->V[x] += kk;
            break;
            
        case 0x8000:
            switch (n) {
                case 0x0: // LD Vx, Vy - Set Vx = Vy
                    chip8->V[x] = chip8->V[y];
                    break;
                    
                case 0x1: // OR Vx, Vy - Set Vx = Vx OR Vy
                    chip8->V[x] |= chip8->V[y];
                    break;
                    
                case 0x2: // AND Vx, Vy - Set Vx = Vx AND Vy
                    chip8->V[x] &= chip8->V[y];
                    break;
                    
                case 0x3: // XOR Vx, Vy - Set Vx = Vx XOR Vy
                    chip8->V[x] ^= chip8->V[y];
                    break;
                    
                case 0x4: // ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry
                    {
                        uint16_t sum = chip8->V[x] + chip8->V[y];
                        chip8->V[0xF] = (sum > 255) ? 1 : 0;
                        chip8->V[x] = sum & 0xFF;
                    }
                    break;
                    
                case 0x5: // SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow
                    chip8->V[0xF] = (chip8->V[x] > chip8->V[y]) ? 1 : 0;
                    chip8->V[x] -= chip8->V[y];
                    break;
                    
                case 0x6: // SHR Vx - Set Vx = Vx SHR 1, set VF = LSB
                    chip8->V[0xF] = chip8->V[x] & 0x1;
                    chip8->V[x] >>= 1;
                    break;
                    
                case 0x7: // SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow
                    chip8->V[0xF] = (chip8->V[y] > chip8->V[x]) ? 1 : 0;
                    chip8->V[x] = chip8->V[y] - chip8->V[x];
                    break;
                    
                case 0xE: // SHL Vx - Set Vx = Vx SHL 1, set VF = MSB
                    chip8->V[0xF] = (chip8->V[x] >> 7) & 0x1;
                    chip8->V[x] <<= 1;
                    break;
            }
            break;
            
        case 0x9000: // SNE Vx, Vy - Skip if Vx != Vy
            if (chip8->V[x] != chip8->V[y]) {
                chip8->PC += 2;
            }
            break;
            
        case 0xA000: // LD I, addr - Set I = nnn
            chip8->I = nnn;
            break;
            
        case 0xB000: // JP V0, addr - Jump to V0 + nnn
            chip8->PC = chip8->V[0] + nnn;
            break;
            
        case 0xC000: // RND Vx, byte - Set Vx = random byte AND kk
            chip8->V[x] = random_byte() & kk;
            break;
            
        case 0xD000: // DRW Vx, Vy, nibble - Draw sprite
            {
                uint8_t vx = chip8->V[x] % DISPLAY_WIDTH;
                uint8_t vy = chip8->V[y] % DISPLAY_HEIGHT;
                uint8_t height = n;
                chip8->V[0xF] = 0;
                
                for (uint8_t row = 0; row < height; row++) {
                    uint8_t sprite_byte = chip8->memory[chip8->I + row];
                    for (uint8_t col = 0; col < 8; col++) {
                        if (sprite_byte & (0x80 >> col)) {
                            int px = (vx + col) % DISPLAY_WIDTH;
                            int py = (vy + row) % DISPLAY_HEIGHT;
                            int idx = py * DISPLAY_WIDTH + px;
                            
                            if (chip8->display[idx] == 0xFFFFFFFF) {
                                chip8->V[0xF] = 1; // Collision
                            }
                            chip8->display[idx] ^= 0xFFFFFFFF; // XOR with white
                        }
                    }
                }
                chip8->draw_flag = true;
            }
            break;
            
        case 0xE000:
            switch (kk) {
                case 0x9E: // SKP Vx - Skip if key with value Vx is pressed
                    if (chip8->keys[chip8->V[x]]) {
                        chip8->PC += 2;
                    }
                    break;
                    
                case 0xA1: // SKNP Vx - Skip if key with value Vx is not pressed
                    if (!chip8->keys[chip8->V[x]]) {
                        chip8->PC += 2;
                    }
                    break;
            }
            break;
            
        case 0xF000:
            switch (kk) {
                case 0x07: // LD Vx, DT - Set Vx = delay timer
                    chip8->V[x] = chip8->delay_timer;
                    break;
                    
                case 0x0A: // LD Vx, K - Wait for key press, store in Vx
                    {
                        bool key_pressed = false;
                        for (uint8_t i = 0; i < NUM_KEYS; i++) {
                            if (chip8->keys[i]) {
                                chip8->V[x] = i;
                                key_pressed = true;
                                break;
                            }
                        }
                        if (!key_pressed) {
                            chip8->PC -= 2; // Stay on this instruction
                        }
                    }
                    break;
                    
                case 0x15: // LD DT, Vx - Set delay timer = Vx
                    chip8->delay_timer = chip8->V[x];
                    break;
                    
                case 0x18: // LD ST, Vx - Set sound timer = Vx
                    chip8->sound_timer = chip8->V[x];
                    break;
                    
                case 0x1E: // ADD I, Vx - Set I = I + Vx
                    chip8->I += chip8->V[x];
                    break;
                    
                case 0x29: // LD F, Vx - Set I = location of sprite for digit Vx
                    chip8->I = chip8->V[x] * 5; // Each sprite is 5 bytes
                    break;
                    
                case 0x33: // LD B, Vx - Store BCD representation of Vx
                    {
                        uint8_t value = chip8->V[x];
                        chip8->memory[chip8->I] = value / 100;
                        chip8->memory[chip8->I + 1] = (value / 10) % 10;
                        chip8->memory[chip8->I + 2] = value % 10;
                    }
                    break;
                    
                case 0x55: // LD [I], Vx - Store V0 to Vx in memory starting at I
                    for (uint8_t i = 0; i <= x; i++) {
                        chip8->memory[chip8->I + i] = chip8->V[i];
                    }
                    break;
                    
                case 0x65: // LD Vx, [I] - Read V0 to Vx from memory starting at I
                    for (uint8_t i = 0; i <= x; i++) {
                        chip8->V[i] = chip8->memory[chip8->I + i];
                    }
                    break;
            }
            break;
    }
}
