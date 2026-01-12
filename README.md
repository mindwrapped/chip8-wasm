# CHIP-8 Emulator (WebAssembly)

A CHIP-8 emulator written in C and compiled to WebAssembly, running in your browser.

## Features

- Full CHIP-8 instruction set implementation
- 64x32 pixel display
- Keyboard input support
- ROM loading from file
- Modern, responsive web interface

## Building

### Prerequisites

- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) installed and configured

### Build Steps

1. Make sure Emscripten is activated:
   ```bash
   source emsdk/emsdk_env.sh  # Adjust path to your Emscripten installation
   ```

2. Build the project:
   ```bash
   make
   ```

This will generate `chip8.js` and `chip8.wasm` files.

## Running Locally

You can test the emulator locally using a simple HTTP server:

```bash
python3 -m http.server 8000
```

Then open `http://localhost:8000` in your browser.

## GitHub Pages Deployment

1. Build the project using the steps above
2. Commit all files including `chip8.js` and `chip8.wasm`
3. Push to your repository
4. Enable GitHub Pages in your repository settings (Settings → Pages)
5. Select the branch containing your files (usually `main` or `master`)
6. Your emulator will be available at `https://<username>.github.io/chip8-wasm/`

## CHIP-8 Keypad Mapping

The CHIP-8 keypad is mapped to your keyboard as follows:

**Primary mapping:**
- `1`, `2`, `3`, `4` → CHIP-8 keys 1, 2, 3, C
- `Q`, `W`, `E`, `R` → CHIP-8 keys 4, 5, 6, D
- `A`, `S`, `D`, `F` → CHIP-8 keys 7, 8, 9, E
- `Z`, `X`, `C`, `V` → CHIP-8 keys A, 0, B, F

**Alternative mapping:**
- Number row: `1-9`, `0`, `-`, `=` → CHIP-8 keys 1-9, 0, A, B
- `Q`, `W`, `E`, `R` → CHIP-8 keys C, D, E, F

You can also use the on-screen keypad by clicking/tapping the buttons.

## ROMs

You can find CHIP-8 ROMs online. Some popular ones include:
- Pong
- Space Invaders
- Tetris
- Breakout
- Snake

Load ROMs using the "Load ROM" button in the interface.

## Technical Details

- **Language**: C
- **Compilation**: Emscripten (WebAssembly)
- **Display**: 64x32 pixels, monochrome
- **Memory**: 4KB RAM
- **Clock Speed**: ~600 instructions per second (10 cycles per frame at 60 FPS)

## License

This project is open source and available for educational purposes.
