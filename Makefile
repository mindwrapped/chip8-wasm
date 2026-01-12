CC = emcc
CFLAGS = -O2 -Wall -Wextra
LDFLAGS = -s WASM=1 \
          -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","UTF8ToString","stringToUTF8","writeArrayToMemory"]' \
          -s ALLOW_MEMORY_GROWTH=1 \
          -s INITIAL_MEMORY=16777216 \
          -s EXPORT_NAME="'Module'" \
          -s MODULARIZE=0 \
          -s EXPORTED_FUNCTIONS='["_malloc","_free","_init_chip8","_load_rom","_run_cycle","_update_timers","_get_display","_get_draw_flag","_set_key","_get_sound_timer","_copy_display","_get_pixel"]' \
          --no-entry

SOURCES = chip8.c main.c
OUTPUT = chip8.js

all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT) $(LDFLAGS)

clean:
	rm -f $(OUTPUT) chip8.wasm

.PHONY: all clean
