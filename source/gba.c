
#include <gba.h>

extern const signed short sinlut[256];

GBA_System*
GBA_GetSystem() {
#ifdef NOGBA
  static char   MEM_IO[0x003FF] = {0};
  static char MEM_VRAM[0x17FFF] = {0};
  static char  MEM_OAM[0x003FF] = {0};
  static char  MEM_PAL[0x003FF] = {0};
#else
  const int MEM_IO   = GBA_MEM_IO;
  const int MEM_VRAM = GBA_MEM_VRAM;
  const int MEM_OAM  = GBA_MEM_OAM;
  const int MEM_PAL  = GBA_MEM_PAL;
#endif

  static GBA_System system = {
    .input = {{.value = 0x03FF}, {.value = 0x03FF}},
    .keypad = GBA_KEYPAD(MEM_IO + 0x0130),

    .directMemcpy0 = GBA_MEMCPY(MEM_IO + 0x00B0),
    .directMemcpy1 = GBA_MEMCPY(MEM_IO + 0x00BC),
    .directMemcpy2 = GBA_MEMCPY(MEM_IO + 0x00C8),
    .directMemcpy3 = GBA_MEMCPY(MEM_IO + 0x00D4),

    .displayControl = GBA_DISPLAY_CONTROL(MEM_IO + 0x0000),
    .backgroundControls = GBA_BACKGROUND_CONTROLS(MEM_IO + 0x0008),
    .backgroundOffsets  = GBA_BACKGROUND_OFFSETS(MEM_IO + 0x0010),

    .blendControl = GBA_BLEND_CONTROL(MEM_IO + 0x0050),
    .blend        = GBA_BLEND(MEM_IO + 0x0052),

    .tileSets4 = GBA_TILESETS4(MEM_VRAM + 0x0000),
    .tileSets8 = GBA_TILESETS8(MEM_VRAM + 0x0000),

    .sprites  = GBA_SPRITES(MEM_OAM + 0x0000),
    .affines  = GBA_AFFINES(MEM_OAM + 0x0000),
    .tileMaps = GBA_TILEMAPS(MEM_VRAM + 0x0000),

    .backgroundPalette = GBA_BACKGROUND_PALETTE(MEM_PAL + 0x0000),
    .spritePalette     = GBA_SPRITE_PALETTE(MEM_PAL + 0x0200),
  };

  return &system;
}

GBA_Input*
GBA_GetInput() {
  GBA_System *system = GBA_GetSystem();
  return &system->input;
}

void
GBA_EnableBackgroundLayer(
    int layer,
    GBA_BackgroundControl control)
{
  GBA_System *system = GBA_GetSystem();

  u16 value = system->displayControl->value;
  value |= 1 << (8 + layer);
  value &= 0b1111111101111111; // unset force blank

  system->displayControl->value = value;
  system->backgroundControls[layer] = control;
}

void
GBA_OffsetBackgroundLayer(
    int layer,
    int px, int py)
{
  GBA_System *system = GBA_GetSystem();

  system->backgroundOffsets[layer] = (GBA_BackgroundOffset) {
    .hOffset = px,
    .vOffset = py,
  };
}

void
GBA_TileMapRef_FromBackgroundLayer(
    GBA_TileMapRef *tileMap,
    int layer)
{
  static const int widths[]  = { 32, 64, 32, 64 };
  static const int heights[] = { 32, 32, 64, 64 };

  GBA_System *system = GBA_GetSystem();
  GBA_BackgroundControl background = system->backgroundControls[layer];

  int mapIndex = background.tileMapIndex;
  int setIndex = background.tileSetIndex;
  int size = background.size;

  GBA_Tile *tiles = system->tileMaps[mapIndex];
  GBA_Bitmap8 *bitmaps = system->tileSets8[setIndex];

  tileMap->width   = widths[size];
  tileMap->height  = heights[size];
  tileMap->tiles   = tiles;
  tileMap->bitmaps = bitmaps;
}

static inline int
GBA_TileMapRef_GetTileIndex(
    GBA_TileMapRef *tileMap,
    int tx, int ty)
{
  int ymask = (tileMap->height - 1);
  int xmask = (tileMap->width - 1);

  ty &= ymask;
  tx &= xmask;

  int index = (ty >> 5) * (tileMap->width >> 5) + (tx >> 5);
  return index*1024 + (ty & 0b011111)*32 + (tx & 0b011111);
}

void
GBA_TileMapRef_BlitTile(
    GBA_TileMapRef *target,
    int tx, int ty,
    const GBA_Tile *tile)
{
  int index = GBA_TileMapRef_GetTileIndex(target, tx, ty);
  target->tiles[index].value = tile->value;
}

void
GBA_TileMapRef_Blit(
    GBA_TileMapRef *target,
    int tx, int ty,
    const GBA_TileMapRef *source)
{
  int endY = (ty + source->height);
  int endX = (tx + source->width);

  int i = 0;
  for (int y = ty; y < endY; y++) {
    for (int x = tx; x < endX; x++) {
      int index = GBA_TileMapRef_GetTileIndex(target, x, y);
      target->tiles[index].value = source->tiles[i++].value;
    }
  }
}

void
GBA_TileMapRef_SetPixel(
    GBA_TileMapRef *tileMap,
    int px, int py, int color)
{
  int tx = px >> 3;
  int ty = py >> 3;

  int tileIndex = GBA_TileMapRef_GetTileIndex(tileMap, tx, ty);
  GBA_Tile *tile = &tileMap->tiles[tileIndex];

  int tileId = tile->tileId;

  GBA_Bitmap8 *bitmap = &tileMap->bitmaps[tileId];
  int mapIndex = ((py & 0b0111) << 1) + ((px & 0b0111) >> 2);

  u32 data = bitmap->data[mapIndex];
  u8 *pixels = (u8 *) &data;

  int index = px & 0b011;
  pixels[index] = color;

  bitmap->data[mapIndex] = data;
}

void
GBA_VSync() {
#ifndef NOGBA
  while(GBA_REG_VCOUNT >= 160); // wait till VDraw
  while(GBA_REG_VCOUNT < 160);  // wait till VBlank
#endif
}

void
GBA_Memcpy(void *dst, const void *src, int size) {
  GBA_System *system = GBA_GetSystem();
  GBA_DirectMemcpy *dma3 = system->directMemcpy3;

  GBA_DirectMemcpy copy = {0};
  copy.dst = dst;
  copy.src = src;
  copy.numTransfers = size >> 2;
  copy.chunkSize = 1; // copy words
  copy.enable = 1;

  dma3->dst = copy.dst;
  dma3->src = copy.src;
  dma3->cnt = copy.cnt;
}

void
GBA_EnableSprites() {
  GBA_System *system = GBA_GetSystem();

  u16 value = system->displayControl->value;
  value |= 1 << 6; // 1D mapping
  value |= 1 << 12; // enable sprites

  system->displayControl->value = value;
}

static inline void
GBA_Sprite_Reset(GBA_Sprite *sprite) {
  static GBA_Sprite empty = { .objMode = 2 }; // hidden

  sprite->attr0 = empty.attr0;
  sprite->attr1 = empty.attr1;
  sprite->attr2 = empty.attr2;
}

void
GBA_Sprite_ResetAll() {
  GBA_System *system = GBA_GetSystem();

  struct GBA_SpriteRef *sprites = &system->spritesRef[0];
  struct GBA_AffineRef *affines = &system->affinesRef[0];

  for (int i = 0; i < 128; i++) {
    struct GBA_SpriteRef *next = &sprites[i + 1];
    struct GBA_Sprite *reference = &system->sprites[i];

    sprites[i].reference = reference;
    sprites[i].next = next;

    GBA_Sprite_Reset(reference);
  }

  sprites[127].next = NULL;

  for (int i = 0; i < 32; i++) {
    struct GBA_AffineRef *next = &affines[i + 1];
    struct GBA_Affine *reference = &system->affines[i];

    affines[i].reference = reference;
    affines[i].next = next;
  }

  affines[31].next = NULL;

  system->unusedSprites = sprites;
  system->unusedAffines = affines;
}

GBA_Sprite*
GBA_Sprite_Allocate() {
  GBA_System *system = GBA_GetSystem();

  struct GBA_SpriteRef *self = system->unusedSprites;
  struct GBA_SpriteRef *next = self->next;

  self->next = NULL;
  system->unusedSprites = next;

  return self->reference;
}

void
GBA_Sprite_Release(GBA_Sprite *sprite) {
  GBA_System *system = GBA_GetSystem();
  GBA_Sprite *sprites = system->sprites;

  int index = ((void *) sprite - (void *) sprites) >> 3;

  struct GBA_SpriteRef *self = &system->spritesRef[index];
  struct GBA_SpriteRef *next = system->unusedSprites;

  self->next = next;
  system->unusedSprites = self;

  GBA_Sprite_Reset(sprite);
}

GBA_Affine*
GBA_Affine_Allocate() {
  GBA_System *system = GBA_GetSystem();

  struct GBA_AffineRef *self = system->unusedAffines;
  struct GBA_AffineRef *next = self->next;

  self->next = NULL;
  system->unusedAffines = next;

  return self->reference;
}

void
GBA_Affine_Release(GBA_Affine *affine) {
  GBA_System *system = GBA_GetSystem();
  GBA_Affine *affines = system->affines;

  int index = ((void *) affine - (void *) affines) >> 5;

  struct GBA_AffineRef *self = &system->affinesRef[index];
  struct GBA_AffineRef *next = system->unusedAffines;

  self->next = next;
  system->unusedAffines = self;
}

void
GBA_Sprite_SetObjMode(
    GBA_Sprite *sprite,
    int mode)
{
  u16 attr0 = sprite->attr0 & 0xFCFF;
  attr0 += (mode & 0x03) << 8;

  sprite->attr0 = attr0;
}

void
GBA_Sprite_SetPosition(
    GBA_Sprite *sprite,
    int px, int py)
{
  u16 attr0 = sprite->attr0 & 0xFF00;
  attr0 += py &  0xFF;

  u16 attr1 = sprite->attr1 & 0xFE00;
  attr1 += px & 0x1FF;

  sprite->attr0 = attr0;
  sprite->attr1 = attr1;
}

void
GBA_Sprite_SetRotation(
    GBA_Sprite *sprite,
    int alpha)
{
  GBA_Affine *affine = GBA_Sprite_GetAffine(sprite);

  if (affine != NULL) {
    s16 sin = sinlut[ alpha & 0xFF];
    s16 cos = sinlut[(alpha - 64) & 0xFF];

    affine->pa =  sin;
    affine->pb = -cos;
    affine->pc =  cos;
    affine->pd =  sin;
  }
}

void
GBA_Sprite_SetPaletteBank(
    GBA_Sprite *sprite,
    int bank)
{
  u16 attr2 = sprite->attr2 & 0x0FFF;
  attr2 += bank << 12;

  sprite->attr2 = attr2;
}

void
GBA_Sprite_SetAffine(
    GBA_Sprite *sprite,
    GBA_Affine *affine,
    bool extended)
{
  GBA_System *system = GBA_GetSystem();
  GBA_Affine *affines = system->affines;

  int index = ((void *) affine - (void *) affines) >> 5;
  sprite->attr0 |= extended ? 0x300 : 0x100; // sets mode to 1 (normal rendering area) or 3 (double rendering area)
  sprite->attr1 |= (index << 9);
}

GBA_Affine*
GBA_Sprite_GetAffine(GBA_Sprite *sprite) {
  GBA_System *system = GBA_GetSystem();
  GBA_Affine *affines = system->affines;

  u16 mode = sprite->objMode;

  if (mode & 0x1) {
    u16 index = sprite->transform;
    GBA_Affine *affine = &affines[index];

    return affine;
  }

  return NULL;
}

void
GBA_Input_PollStates(GBA_Input *input) {
  GBA_System *system = GBA_GetSystem();
  GBA_Keypad *keypad = system->keypad;

  input->previous.value = input->current.value;
  input->current.value = ~(keypad->value);
}

bool
GBA_Input_IsPressed(
    GBA_Input *input,
    GBA_Key key)
{
  return (input->current.value & key) > 0;
}

bool
GBA_Input_IsHit(
    GBA_Input *input,
    GBA_Key key)
{
  return ((input->current.value &~ input->previous.value) & key) > 0;
}

bool
GBA_Input_IsHeld(
    GBA_Input *input,
    GBA_Key key)
{
  return ((input->current.value & input->previous.value) & key) > 0;
}
