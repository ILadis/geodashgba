
ifndef DEVKITPRO
  $(error Please set DEVKITPRO in your environment)
endif

PATH     := $(DEVKITPRO)/tools/bin:$(DEVKITPRO)/devkitARM/bin:$(PATH)

# source files
CFILES   := $(wildcard source/*.c) $(wildcard source/game/*.c) $(wildcard source/scenes/*.c)
CFILES   += $(wildcard source/game/collection/*.c) $(wildcard source/game/cube/*.c) $(wildcard source/game/level/*.c) $(wildcard source/game/object/*.c)
CFILES   += $(wildcard assets/*.c) $(wildcard assets/fonts/*.c) $(wildcard assets/graphics/*.c) $(wildcard assets/tiles/*.c)

# level files
LEVELS   := $(patsubst %.txt,%.bin,$(wildcard levels/*.txt))
LEVELS   := $(filter-out levels/99_%.bin, $(LEVELS))

# toolchain and flags
EMU      := visualboyadvance-m
CC       := arm-none-eabi-gcc
OBJCOPY  := arm-none-eabi-objcopy

ARCH     := -mthumb-interwork -mthumb
SPECS    := -specs=gba.specs

CFLAGS   := $(ARCH) -O2 -Wall -Wextra -fno-strict-aliasing -nostdinc -mcpu=arm7tdmi -mtune=arm7tdmi -fomit-frame-pointer -ffast-math -Iinclude
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

.PHONY : tools utils assets build run clean purge

build: main.gba main.elf

run:
	@$(EMU) main.gba

clean:
	@rm -rf *.gba *.elf

purge: clean
	@rm -rf assets/ levels/*.bin \
		tools/sinlut    \
		tools/bezlut    \
		tools/ppm2font  \
		tools/tmx2tiles \
		tools/lvl2bin   \
		tools/lvl2rom

tools:
	@gcc tools/sinlut.c -o tools/sinlut -lm
	@gcc tools/bezlut.c -o tools/bezlut -lm
	@gcc tools/ppm2font.c -o tools/ppm2font
	@gcc tools/tmx2tiles.c -o tools/tmx2tiles

utils: CFILES := $(filter-out source/main.c, $(CFILES))
utils:
	@gcc tools/lvl2bin.c $(CFILES) -o tools/lvl2bin -lm -Iinclude -DNOGBA
	@gcc tools/lvl2rom.c $(CFILES) -o tools/lvl2rom -lm -Iinclude -DNOGBA
	@gcc tools/mksnd.c $(CFILES) -o tools/mksnd -lm -Iinclude -DNOGBA
	@gcc tools/lsdsk.c $(CFILES) -o tools/lsdsk -lm -Iinclude -DNOGBA

assets:
	@mkdir -p assets/graphics assets/tiles
	@grit graphics/tiles.bmp -o assets/graphics/tiles -gB8 -Mw 1 -Mh 1 -ftc -gT0
	@grit graphics/sprites.bmp -o assets/graphics/sprites -gB4 -Mw 1 -Mh 1 -ftc -gT0
	@tools/tmx2tiles < graphics/backgrounds.tmx > assets/tiles/backgrounds.c
	@tools/tmx2tiles < graphics/snippets.tmx > assets/tiles/snippets.c
	@tools/sinlut > assets/sinlut.c
	@tools/bezlut 0.19 1 0.22 1 > assets/bezlut.c
	@tools/ppm2font 6x15 hud < graphics/font.ppm > assets/font.c
	@tools/ppm2font 8x8 console < graphics/console.ppm > assets/console.c

levels: $(LEVELS)
levels/%.bin: levels/%.txt main.gba
	@tools/lvl2bin $< > $@
	@tools/lvl2rom main.gba < $@

tests: $(TESTS)

test/%: CFILES := $(filter-out source/main.c, $(CFILES))
test/%:
	@gcc $@.c $(CFILES) -o test.elf -g -O0 -lm -Iinclude -DNOGBA
	@$(RUNNER) ./test.elf
	@rm test.elf

%.gba: %.elf
	@$(OBJCOPY) -v -O binary $< $@
	@gbafix $@

%.elf:
	@$(CC) $(CFLAGS) $(CFILES) -o $@ $(LDFLAGS)
