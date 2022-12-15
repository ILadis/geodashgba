
ifeq ($(strip $(DEVKITPRO)),)
  $(info Please set DEVKITPRO in your environment)
endif

PATH     := $(DEVKITPRO)/tools/bin:$(DEVKITPRO)/devkitARM/bin:$(PATH)

# source and target files
CFILES   := $(wildcard source/*.c) $(wildcard source/game/*.c) $(wildcard source/scenes/*.c)
CFILES   += $(wildcard assets/*.c) $(wildcard assets/fonts/*.c) $(wildcard assets/graphics/*.c)
TFILES   := $(filter-out source/main.c, $(CFILES))

# toolchain and flags
EMU      := visualboyadvance-m
CC       := arm-none-eabi-gcc
OBJCOPY  := arm-none-eabi-objcopy

ARCH     := -mthumb-interwork -mthumb
SPECS    := -specs=gba.specs

CFLAGS   := $(ARCH) -O2 -Wall -fno-strict-aliasing -nostdinc -I. -Iinclude
LDFLAGS  := $(ARCH) $(SPECS)


.PHONY : assets tools run build clean

run: build
	$(EMU) main.gba

build: main.gba

clean:
	@rm -rf *.gba *.elf

tools:
	@gcc tools/sinlut.c -o tools/sinlut -lm

assets:
	@mkdir -p assets/graphics
	@tools/sinlut > assets/sinlut.c
	@grit graphics/tiles.bmp -o assets/graphics/tiles -gB8 -Mw 1 -Mh 1 -ftc -gT0
	@grit graphics/sprites.bmp -o assets/graphics/sprites -gB4 -Mw 1 -Mh 1 -ftc -gT0
	@tiled --export-map 'GBA Tilemap C-Source file' tools/editor/maps/background.tmx assets/background.c || true

tests: gba_test grid_test hit_test math_test prefab_test scene_test vector_test loader_game_test

debug: CFLAGS += -O0 -g
debug: clean main.gba main.elf
	@mgba -3 --gdb main.gba

%_game_test: test/game/%.c
	@gcc $< $(TFILES) -o $@.elf -g -I. -Iinclude -DNOGBA
	@./$@.elf
	@rm $@.elf

%_test: test/%.c
	@gcc $< $(TFILES) -o $@.elf -g -I. -Iinclude -DNOGBA
	@./$@.elf
	@rm $@.elf

debug_%_test: test/%.c
	@gcc $< $(TFILES) -o test_debug.elf -g -I. -Iinclude -DNOGBA
	@gdbserver :2345 ./test_debug.elf

%.gba: %.elf
	@$(OBJCOPY) -v -O binary $< $@
	@gbafix $@

%.elf:
	@$(CC) $(CFLAGS) $(CFILES) -o $@ $(LDFLAGS)
