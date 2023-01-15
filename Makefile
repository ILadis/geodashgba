
ifndef DEVKITPRO
  $(error Please set DEVKITPRO in your environment)
endif

PATH     := $(DEVKITPRO)/tools/bin:$(DEVKITPRO)/devkitARM/bin:$(PATH)

# source files
CFILES   := $(wildcard source/*.c) $(wildcard source/game/*.c) $(wildcard source/scenes/*.c)
CFILES   += $(wildcard source/game/cube/*.c) $(wildcard source/game/level/*.c) $(wildcard source/game/object/*.c)
CFILES   += $(wildcard assets/*.c) $(wildcard assets/fonts/*.c) $(wildcard assets/graphics/*.c) $(wildcard assets/tiles/*.c)

# toolchain and flags
EMU      := visualboyadvance-m
CC       := arm-none-eabi-gcc
OBJCOPY  := arm-none-eabi-objcopy

ARCH     := -mthumb-interwork -mthumb
SPECS    := -specs=gba.specs

CFLAGS   := $(ARCH) -O2 -Wall -fno-strict-aliasing -nostdinc -mcpu=arm7tdmi -mtune=arm7tdmi -fomit-frame-pointer -ffast-math -I. -Iinclude
LDFLAGS  := $(ARCH) $(SPECS)

# test cases
TESTS    := $(basename $(wildcard test/*.c) $(wildcard test/game/*.c))
RUNNER   := exec

# debug settings
ifdef DEBUG
  CFLAGS  += -O0 -g
  LDFLAGS += -g
  RUNNER  := gdbserver :2345
  EMU     := mgba -l 8 -3 --gdb
endif

.PHONY : assets tools run build clean

run: build
	@$(EMU) main.gba

build: main.gba main.elf

clean:
	@rm -rf *.gba *.elf

purge: clean
	@rm -rf assets/ tools/sinlut tools/lvl2bin

tools: CFILES := $(filter-out source/main.c, $(CFILES))
tools:
	@gcc tools/sinlut.c -o tools/sinlut -lm
	@gcc tools/bezlut.c -o tools/bezlut -lm
	@gcc tools/lvl2bin.c $(CFILES) -o tools/lvl2bin -g -I. -Iinclude -DNOGBA

assets:
	@mkdir -p assets/graphics assets/tiles
	@grit graphics/tiles.bmp -o assets/graphics/tiles -gB8 -Mw 1 -Mh 1 -ftc -gT0
	@grit graphics/sprites.bmp -o assets/graphics/sprites -gB4 -Mw 1 -Mh 1 -ftc -gT0
	@tiled --export-map 'GBA Tilemap C-Source file' tools/editor/maps/backgrounds.tmx assets/tiles/backgrounds.c || true
	@tiled --export-map 'GBA Tilemap C-Source file' tools/editor/maps/overlays.tmx assets/tiles/overlays.c || true
	@tools/sinlut > assets/sinlut.c
	@tools/bezlut 0.19 1 0.22 1 > assets/bezlut.c
	@tools/lvl2bin > assets/levels.c

tests: $(TESTS)

test/%: CFILES := $(filter-out source/main.c, $(CFILES))
test/%:
	@gcc $@.c $(CFILES) -o test.elf -g -O0 -I. -Iinclude -DNOGBA
	@$(RUNNER) ./test.elf
	@rm test.elf

%.gba: %.elf
	@$(OBJCOPY) -v -O binary $< $@
	@gbafix $@

%.elf:
	@$(CC) $(CFLAGS) $(CFILES) -o $@ $(LDFLAGS)
