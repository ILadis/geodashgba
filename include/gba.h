#ifndef GBA_H
#define GBA_H

#include <types.h>

#define GBA_SCREEN_WIDTH  240
#define GBA_SCREEN_HEIGHT 160

#define GBA_SCREEN_COLS 30
#define GBA_SCREEN_ROWS 20

#define GBA_MEM_WRAM 0x02000000
#define GBA_MEM_IRAM 0x03000000
#define GBA_MEM_IO   0x04000000
#define GBA_MEM_PAL  0x05000000 // only writable in 16/32-bit chunks
#define GBA_MEM_VRAM 0x06000000 // -"-
#define GBA_MEM_OAM  0x07000000 // -"-
#define GBA_MEM_ROM  0x08000000

typedef enum GBA_Interrupt {
  GBA_IRQ_VBLANK,
  GBA_IRQ_HBLANK,
  GBA_IRQ_VCOUNT,
  GBA_IRQ_TIMER1,
  GBA_IRQ_TIMER2,
  GBA_IRQ_TIMER3,
  GBA_IRQ_TIMER4,
  GBA_IRQ_SERIAL,
  GBA_IRQ_DMA1,
  GBA_IRQ_DMA2,
  GBA_IRQ_DMA3,
  GBA_IRQ_DMA4,
  GBA_IRQ_INPUT,
} GBA_Interrupt;

typedef union GBA_InterruptControl {
  u16 value;
  struct {
    u16 vblank: 1;
    u16 hblank: 1;
    u16 vcount: 1;
    u16 timer1: 1;
    u16 timer2: 1;
    u16 timer3: 1;
    u16 timer4: 1;
    u16 serial: 1;
    u16 dma1: 1;
    u16 dma2: 1;
    u16 dma3: 1;
    u16 dma4: 1;
    u16 input: 1;
  };
} GBA_InterruptControl;

typedef struct GBA_InterruptStatus {
  u16 value;
} GBA_InterruptStatus;

typedef void (*GBA_InterruptHandlerFn)(void);

#define GBA_INTERRUPT_CONTROL(ADDR) ((GBA_InterruptControl *)   (ADDR))
#define GBA_INTERRUPT_STATUS(ADDR)  ((GBA_InterruptStatus *)    (ADDR))
#define GBA_INTERRUPT_HANDLER(ADDR) ((GBA_InterruptHandlerFn *) (ADDR))

typedef struct GBA_DisplayVCount {
  u16 value;
} GBA_DisplayVCount;

typedef union GBA_DisplayControl {
  u16 value;
  struct {
    u16 mode: 3;
    u16 isGBC: 1;
    u16 pageSelect: 1;
    u16 oamHBlank: 1;
    u16 sprite1DMapping: 1;
    u16 forceBlank: 1;
    u16 enableBG0: 1;
    u16 enableBG1: 1;
    u16 enableBG2: 1;
    u16 enableBG3: 1;
    u16 enableOBJ: 1;
    u16 enableWIN0: 1;
    u16 enableWIN1: 1;
    u16 enableWINOBJ: 1;
  };
} GBA_DisplayControl;

/* Available display modes:
 *
 * | Mode |   BG0   |   BG1   |   BG2   |   BG3   |
 * +------+---------+---------+---------+---------+
 * |  0   | regular | regular | regular | regular |
 * |  1   | regular | regular | affine  |    -    |
 * |  2   |    -    |    -    | affine  | affine  |
 * | Mode |  Width  | Height  |  Bits per Pixel   | Page flip |
 * +------+---------+---------+-------------------+-----------+
 * |  3   |   240   |   160   |        16         |    no     |
 * |  4   |   240   |   160   |         8         |    yes    |
 * |  5   |   160   |   128   |        16         |    yes    |
 */

typedef union GBA_BackgroundControl {
  u16 value;
  struct {
    // background with highest priority is drawn first
    u16 priority: 2;
    u16 tileSetIndex: 2;
    u16 unused: 2;
    u16 mosaic: 1;
    u16 colorMode: 1;
    u16 tileMapIndex: 5;
    u16 affineWrapping: 1;
    u16 size: 2;
  };
} GBA_BackgroundControl;

/* Available regular background sizes:
 *
 * | Size flag | Tiles | Pixels  |
 * +-----------+-------+---------+
 * |    00     | 32x32 | 256x256 |
 * |    01     | 64x32 | 512x256 |
 * |    10     | 32x64 | 256x512 |
 * |    11     | 64x64 | 512x512 |
 */

/* Available affine background sizes:
 *
 * | Size flag | Tiles   | Pixels    |
 * +-----------+---------+-----------+
 * |    00     | 16x16   | 128x128   |
 * |    01     | 32x32   | 256x256   |
 * |    10     | 64x64   | 512x512   |
 * |    11     | 128x128 | 1024x1024 |
 */

typedef struct GBA_BackgroundOffset {
  s16 hOffset;
  s16 vOffset;
} GBA_BackgroundOffset;

typedef union GBA_BlendControl {
  u16 value;
  struct {
    u16 aBG0: 1;
    u16 aBG1: 1;
    u16 aBG2: 1;
    u16 aBG3: 1;
    u16 aOBJ: 1;
    u16 aBD: 1;
    u16 blendingMode: 2;
    u16 bBG0: 1;
    u16 bBG1: 1;
    u16 bBG2: 1;
    u16 bBG3: 1;
    u16 bOBJ: 1;
    u16 bBD: 1;
  };
} GBA_BlendControl;

typedef union GBA_Blend {
  u16 value;
  struct {
    u16 eva: 5;
    u16 unused: 3;
    u16 evb: 5;
  };
} GBA_Blend;

typedef union GBA_BlendFace {
  u16 value;
  struct {
    u16 ey: 5;
  };
} GBA_BlendFace;

typedef enum GBA_Key {
  GBA_KEY_A = 0x0001,
  GBA_KEY_B = 0x0002,
  GBA_KEY_SELECT = 0x0004,
  GBA_KEY_START  = 0x0008,
  GBA_KEY_RIGHT  = 0x0010,
  GBA_KEY_LEFT   = 0x0020,
  GBA_KEY_UP     = 0x0040,
  GBA_KEY_DOWN   = 0x0080,
  GBA_KEY_R = 0x0100,
  GBA_KEY_L = 0x0200,
} GBA_Key;

typedef union GBA_Keypad {
  u16 value;
  struct {
    u16 a: 1;
    u16 b: 1;
    u16 select: 1;
    u16 start: 1;
    u16 right: 1;
    u16 left: 1;
    u16 up: 1;
    u16 down: 1;
    u16 r: 1;
    u16 l: 1;
  };
} GBA_Keypad;

#define GBA_DISPLAY_VCOUNT(ADDR)     ((GBA_DisplayVCount *)     (ADDR))
#define GBA_DISPLAY_CONTROL(ADDR)    ((GBA_DisplayControl *)    (ADDR))
#define GBA_BACKGROUND_CONTROL(ADDR) ((GBA_BackgroundControl *) (ADDR))
#define GBA_BACKGROUND_OFFSET(ADDR)  ((GBA_BackgroundOffset *)  (ADDR))
#define GBA_BLEND_CONTROL(ADDR)      ((GBA_BlendControl *)      (ADDR))
#define GBA_BLEND_FACE(ADDR)         ((GBA_BlendFace *)         (ADDR))
#define GBA_BLEND(ADDR)              ((GBA_Blend *)             (ADDR))
#define GBA_KEYPAD(ADDR)             ((GBA_Keypad *)            (ADDR))

typedef struct GBA_Input {
  GBA_Keypad previous;
  GBA_Keypad current;
} GBA_Input;

typedef struct GBA_TimerData {
  u16 value;
} GBA_TimerData;

typedef enum GBA_TimerFrequency {
  GBA_TIMER_FREQUENCY_1    = 0,
  GBA_TIMER_FREQUENCY_64   = 1,
  GBA_TIMER_FREQUENCY_256  = 2,
  GBA_TIMER_FREQUENCY_1024 = 3,
} GBA_TimerFrequency;

typedef union GBA_TimerControl {
  u16 value;
  struct {
    u16 frequency: 2;
    u16 cascade: 1;
    u16 unused: 3;
    u16 interrupt: 1;
    u16 enable: 1;
  };
} GBA_TimerControl;

#define GBA_TIMER_DATA(ADDR)    ((GBA_TimerData *)    (ADDR))
#define GBA_TIMER_CONTROL(ADDR) ((GBA_TimerControl *) (ADDR))

typedef union GBA_SoundControl {
  u16 value;
  struct {
    u16 unused1: 2;
    u16 soundARatio: 1; // volume ratio 50% (if clear) or 100% (if set)
    u16 soundBRatio: 1;
    u16 unused2: 4;
    u16 soundAEnable: 2;// enable sound on right and/or left speaker
    u16 soundATimer: 1; // use timer 0 (if clear) or timer 1 (if set)
    u16 soundAReset: 1;
    u16 soundBEnable: 2;
    u16 soundBTimer: 1;
    u16 soundBReset: 1;
  };
} GBA_SoundControl;

typedef struct GBA_SoundStatus {
  u16 value;
} GBA_SoundStatus;

typedef struct GBA_SoundData {
  u32 value;
} GBA_SoundData;

#define GBA_SOUND_CONTROL(ADDR) ((GBA_SoundControl *) (ADDR))
#define GBA_SOUND_STATUS(ADDR)  ((GBA_SoundStatus *)  (ADDR))
#define GBA_SOUND_DATA(ADDR)    ((GBA_SoundData *)    (ADDR))

typedef union GBA_Color {
  u16 value;
  struct {
    u16 red: 5;
    u16 green: 5;
    u16 blue: 5;
  };
} GBA_Color;

#define GBA_Color_From(rgb) ((GBA_Color) { \
  .red   = round(((rgb >> 16) & 0xFF) * (31/255.0)), \
  .green = round(((rgb >>  8) & 0xFF) * (31/255.0)), \
  .blue  = round(((rgb >>  0) & 0xFF) * (31/255.0)), \
})

static inline bool
GBA_Color_Equals(GBA_Color color, GBA_Color other) {
  return color.red == other.red
      && color.green == other.green
      && color.blue == other.blue;
}

typedef enum GBA_SpriteMode {
  GBA_SPRITE_MODE_RENDER  = 0,
  GBA_SPRITE_MODE_HIDE    = 2,
  GBA_SPRITE_MODE_AFFINE  = 1,
  GBA_SPRITE_MODE_AFFINE2 = 3,
} GBA_SpriteObjectMode;

#define GBA_SPRITE_SHAPE_OF(w, h) (w == h ? 0 : w > h ? 1 : 2)
#define GBA_SPRITE_SIZE_OF(w, h)  (w*h <= 128 ? 0 : w*h <= 256 ? 1 : w*h <= 1024 ? 2 : 3)

typedef struct GBA_Sprite {
  union {
    u16 attr0;
    struct {
      u16 posY: 8;
      u16 objMode: 2;
      u16 gfxMode: 2;
      u16 mosaic: 1;
      u16 colorMode: 1;
      u16 shape: 2;
    };
  };
  union {
    u16 attr1;
    struct {
      u16 posX: 9;
      u16 transform: 5;
      u16 size: 2;
    };
  };
  union {
    u16 attr2;
    struct {
      u16 tileId: 10;
      u16 priority: 2;
      u16 paletteBank: 4;
    };
  };
  s16 fill;
} align4 GBA_Sprite;

typedef struct GBA_Affine {
  u16 fill0[3];
  s16 pa;
  u16 fill1[3];
  s16 pb;
  u16 fill2[3];
  s16 pc;
  u16 fill3[3];
  s16 pd;
} align4 GBA_Affine;

typedef union GBA_Tile {
  u16 value;
  struct {
    u16 tileId: 10;
    u16 hFlip: 1;
    u16 vFlip: 1;
    u16 paletteBank: 4;
  };
} GBA_Tile;

/* GBA_TileSet4/8 and GBA_TileMap overlap in memory:
 *
 * | Memory Address | 0600:0000 | 0600:4000 | 0600:8000 | 0600:C000 |
 * | Tileset Index  |     0     |     1     |     2     |     3     |
 * | Tilemap Index  |   0 … 7   |   8 … 15  |  16 … 23  |  24 … 31  |
 * 
 * GBA_Tiles for GBA_Sprites follow in memory:
 *
 * | Memory Address | 0601:0000 | 0601:4000 |
 * | Tile Index     |    0…511  |  512…1023 |
 */

typedef GBA_Tile GBA_TileMap[1024];

#define GBA_Tiles_Of(tiles...) ((GBA_Tile *) ((const GBA_Tile[]) { tiles }))

/* GBA_Bitmap8: the pixel value is the palette-index for that pixel.
 * GBA_Bitmap4: the pixel value is the lower nybble of the palette-index (the upper nybble is stored in GBA_Tile/GBA_Sprite).
 */
typedef GBA_Color GBA_Palette[256];

typedef struct { u32 data[8];  } GBA_Bitmap4; // 4bpp (32 bytes)
typedef struct { u32 data[16]; } GBA_Bitmap8; // 8bpp (64 bytes)

typedef GBA_Bitmap4 GBA_TileSet4[512];
typedef GBA_Bitmap8 GBA_TileSet8[256];

#define GBA_COLORS(ADDR)   ((GBA_Color   *) (ADDR))
#define GBA_SPRITES(ADDR)  ((GBA_Sprite  *) (ADDR))
#define GBA_AFFINES(ADDR)  ((GBA_Affine  *) (ADDR))
#define GBA_TILEMAPS(ADDR) ((GBA_TileMap *) (ADDR))

#define GBA_TILESETS4(ADDR) ((GBA_TileSet4 *) (ADDR))
#define GBA_TILESETS8(ADDR) ((GBA_TileSet8 *) (ADDR))

#define GBA_BACKGROUND_PALETTE(ADDR) ((GBA_Palette *) (ADDR))
#define GBA_SPRITE_PALETTE(ADDR)     ((GBA_Palette *) (ADDR))

typedef struct GBA_DirectMemcpy {
  const void *src;
  void *dst;
  union {
    u32 cnt;
    struct {
      u32 numTransfers: 16;
      u32 unused1: 5;
      u32 dstAdjust: 2;
      u32 srcAdjust: 2;
      u32 repeat: 1;
      u32 chunkSize: 1;
      u32 unused2: 1;
      u32 timingMode: 2;
      u32 interruptReq: 1;
      u32 enable: 1;
    };
  };
} GBA_DirectMemcpy;

#define GBA_MEMCPY(ADDR) ((GBA_DirectMemcpy *) (ADDR))

typedef struct GBA_TileMapRef {
  int width, height;
  GBA_Tile *tiles;
  GBA_Bitmap8 *bitmaps;
} GBA_TileMapRef;

typedef struct GBA_System {
  GBA_Input input;
  GBA_Keypad *const volatile keypad;

  GBA_InterruptControl *const volatile interruptControl;
  GBA_InterruptControl *const volatile interruptAcks[2];
  GBA_InterruptStatus *const volatile interruptStatus;
  GBA_InterruptHandlerFn *const volatile interruptHandler;

  GBA_TimerData *const volatile timerData[4];
  GBA_TimerControl *const volatile timerControl[4];

  GBA_SoundControl *const volatile soundControl;
  GBA_SoundStatus *const volatile soundStatus;
  GBA_SoundData *const volatile soundDataA;
  GBA_SoundData *const volatile soundDataB;

  GBA_DirectMemcpy *const volatile directMemcpy[4];

  GBA_DisplayVCount *const volatile vcount;
  GBA_DisplayControl *const volatile displayControl;
  GBA_BackgroundControl *const volatile backgroundControls[4];
  GBA_BackgroundOffset *const volatile backgroundOffsets[4];

  GBA_BlendControl *const volatile blendControl;
  GBA_BlendFace *const volatile blendFace;
  GBA_Blend *const volatile blend;

  GBA_Color *const pixels;
  GBA_TileSet4 *const tileSets4;
  GBA_TileSet8 *const tileSets8;

  struct GBA_SpriteRef {
    struct GBA_Sprite *reference;
    struct GBA_SpriteRef *next;
  } spritesRef[128], *unusedSprites;

  struct GBA_AffineRef {
    struct GBA_Affine *reference;
    struct GBA_AffineRef *next;
  } affinesRef[32], *unusedAffines;

  GBA_Sprite  *const sprites;
  GBA_Affine  *const affines;
  GBA_TileMap *const tileMaps;

  GBA_Palette *const backgroundPalette;
  GBA_Palette *const spritePalette;
} GBA_System;

GBA_System*
GBA_GetSystem();

void
GBA_EnableInterrupt(GBA_Interrupt interrupt);

void
GBA_EnableMode(int mode);

void
GBA_EnableBackgroundLayer(
    int layer,
    GBA_BackgroundControl control);

void
GBA_DisableBackgroundLayer(int layer);

void
GBA_OffsetBackgroundLayer(
    int layer,
    int px, int py);

void
GBA_StartTimerCascade(
    GBA_TimerFrequency frequency,
    const GBA_TimerData *overflows);

int
GBA_GetTimerValue(
    int index,
    const GBA_TimerData *overflows);

void
GBA_EnableSound();

void
GBA_DisableSound();

void
GBA_TileMapRef_FromBackgroundLayer(
    GBA_TileMapRef *tileMap,
    int layer);

void
GBA_TileMapRef_BlitTile(
    GBA_TileMapRef *target,
    int tx, int ty,
    const GBA_Tile *tile);

void
GBA_TileMapRef_Blit(
    GBA_TileMapRef *target,
    int tx, int ty,
    const GBA_TileMapRef *source);

void
GBA_TileMapRef_FillTile(
    GBA_TileMapRef *tileMap,
    int tileId, int color);

void
GBA_TileMapRef_SetPixel(
    GBA_TileMapRef *tileMap,
    int px, int py, int color);

void
GBA_VSync();

void
GBA_VSyncWait();

void
GBA_Memcpy32(void *dst, const void *src, int size);

void
GBA_Memcpy16(void *dst, const void *src, int size);

void
GBA_Memset32(void *dst, int value, int size);

void
GBA_Memset16(void *dst, short value, int size);

void
GBA_EnableSprites();

void
GBA_Sprite_ResetAll();

GBA_Sprite*
GBA_Sprite_Allocate();

void
GBA_Sprite_Release(GBA_Sprite *sprite);

GBA_Affine*
GBA_Affine_Allocate();

void
GBA_Affine_Release(GBA_Affine *affine);

void
GBA_Sprite_SetObjMode(
    GBA_Sprite *sprite,
    GBA_SpriteObjectMode mode);

void
GBA_Sprite_SetPosition(
    GBA_Sprite *sprite,
    int px, int py);

void
GBA_Sprite_SetRotation(
    GBA_Sprite *sprite,
    int alpha);

void
GBA_Sprite_SetTileId(
    GBA_Sprite *sprite,
    int tileId);

void
GBA_Sprite_SetVFlip(
    GBA_Sprite *sprite,
    bool flip);

void
GBA_Sprite_SetHFlip(
    GBA_Sprite *sprite,
    bool flip);

void
GBA_Sprite_SetPaletteBank(
    GBA_Sprite *sprite,
    int bank);

void
GBA_Sprite_SetAffine(
    GBA_Sprite *sprite,
    GBA_Affine *affine,
    bool extended);

GBA_Affine*
GBA_Sprite_GetAffine(GBA_Sprite *sprite);

int
GBA_Palette_FindColor(GBA_Color color);

int
GBA_Bitmap8_GetPixel(
    GBA_Bitmap8 *bitmap,
    int px, int py);

void
GBA_Bitmap_FillPixel(
    int px, int py,
    GBA_Color color);

void
GBA_Bitmap_FillRect(
    int px, int py,
    int width, int height,
    GBA_Color color);

void
GBA_Bitmap_DrawRect(
    int px1, int py1,
    int px2, int py2,
    GBA_Color color);

void
GBA_Bitmap_DrawLine(
    int px1, int py1,
    int px2, int py2,
    GBA_Color color);

void
GBA_Input_PollStates();

bool
GBA_Input_IsPressed(GBA_Key key);

bool
GBA_Input_IsHit(GBA_Key key);

bool
GBA_Input_IsHeld(GBA_Key key);

#endif
