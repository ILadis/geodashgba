
#include <gba.h>

GBA_System*
GBA_GetSystem() {
#ifdef NOGBA
  static char MEM_IRAM[0x07FFF] = {0};
  static char   MEM_IO[0x003FF] = {0};
  static char MEM_VRAM[0x17FFF] = {0};
  static char  MEM_OAM[0x003FF] = {0};
  static char  MEM_PAL[0x003FF] = {0};
#else
  const int MEM_IRAM = GBA_MEM_IRAM;
  const int MEM_IO   = GBA_MEM_IO;
  const int MEM_VRAM = GBA_MEM_VRAM;
  const int MEM_OAM  = GBA_MEM_OAM;
  const int MEM_PAL  = GBA_MEM_PAL;
#endif

  static GBA_System system = {
    .input = {{.value = 0x03FF}, {.value = 0x03FF}},
    .keypad = GBA_KEYPAD(MEM_IO + 0x0130),

    .interruptControl = GBA_INTERRUPT_CONTROL(MEM_IO + 0x0200),
    .interruptAcks[0] = GBA_INTERRUPT_CONTROL(MEM_IO + 0x0202),
    .interruptAcks[1] = GBA_INTERRUPT_CONTROL(MEM_IRAM + 0x7FF8),
    .interruptStatus = GBA_INTERRUPT_STATUS(MEM_IO + 0x0208),
    .interruptHandler = GBA_INTERRUPT_HANDLER(MEM_IRAM + 0x7FFC),

    .timerData[0] = GBA_TIMER_DATA(MEM_IO + 0x0100),
    .timerData[1] = GBA_TIMER_DATA(MEM_IO + 0x0104),
    .timerData[2] = GBA_TIMER_DATA(MEM_IO + 0x0108),
    .timerData[3] = GBA_TIMER_DATA(MEM_IO + 0x010C),

    .timerControl[0] = GBA_TIMER_CONTROL(MEM_IO + 0x0102),
    .timerControl[1] = GBA_TIMER_CONTROL(MEM_IO + 0x0106),
    .timerControl[2] = GBA_TIMER_CONTROL(MEM_IO + 0x010A),
    .timerControl[3] = GBA_TIMER_CONTROL(MEM_IO + 0x010E),

    .soundControl = GBA_SOUND_CONTROL(MEM_IO + 0x0082),
    .soundStatus = GBA_SOUND_STATUS(MEM_IO + 0x0084),
    .soundDataA = GBA_SOUND_DATA(MEM_IO + 0x00A0),
    .soundDataB = GBA_SOUND_DATA(MEM_IO + 0x00A4),

    .directMemcpy[0] = GBA_MEMCPY(MEM_IO + 0x00B0),
    .directMemcpy[1] = GBA_MEMCPY(MEM_IO + 0x00BC),
    .directMemcpy[2] = GBA_MEMCPY(MEM_IO + 0x00C8),
    .directMemcpy[3] = GBA_MEMCPY(MEM_IO + 0x00D4),

    .vcount = GBA_DISPLAY_VCOUNT(MEM_IO + 0x0006),
    .displayControl = GBA_DISPLAY_CONTROL(MEM_IO + 0x0000),

    .backgroundControls[0] = GBA_BACKGROUND_CONTROL(MEM_IO + 0x0008),
    .backgroundControls[1] = GBA_BACKGROUND_CONTROL(MEM_IO + 0x000A),
    .backgroundControls[2] = GBA_BACKGROUND_CONTROL(MEM_IO + 0x000C),
    .backgroundControls[3] = GBA_BACKGROUND_CONTROL(MEM_IO + 0x000E),

    .backgroundOffsets[0]  = GBA_BACKGROUND_OFFSET(MEM_IO + 0x0010),
    .backgroundOffsets[1]  = GBA_BACKGROUND_OFFSET(MEM_IO + 0x0014),
    .backgroundOffsets[2]  = GBA_BACKGROUND_OFFSET(MEM_IO + 0x0018),
    .backgroundOffsets[3]  = GBA_BACKGROUND_OFFSET(MEM_IO + 0x001C),

    .blendControl = GBA_BLEND_CONTROL(MEM_IO + 0x0050),
    .blendFace    = GBA_BLEND_FACE(MEM_IO + 0x0054),
    .blend        = GBA_BLEND(MEM_IO + 0x0052),

    .pixels    = GBA_COLORS(MEM_VRAM + 0x0000),
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

static arm iwram void
GBA_InterruptHandler() {
  GBA_System *system = GBA_GetSystem();

  GBA_InterruptStatus *status = system->interruptStatus;
  status->value = 0;

  GBA_InterruptControl *control = system->interruptControl;

  GBA_InterruptControl *irqack  = system->interruptAcks[0];
  GBA_InterruptControl *irqbios = system->interruptAcks[1];

  u32 ack = control->value & irqack->value;
  irqack->value = ack;
  irqbios->value |= ack;

  // TODO implement dispatching to interrupt service routines

  status->value = 1;
}

void
GBA_EnableInterrupt(GBA_Interrupt interrupt) {
  static const struct {
    u16 offset;
    u16 flag;
  } senders[] = {
    { 0x0004, 0x0008 },
    { 0x0004, 0x0010 },
    { 0x0004, 0x0020 },
    { 0x0102, 0x0040 },
    { 0x0106, 0x0040 },
    { 0x010A, 0x0040 },
    { 0x010E, 0x0040 },
    { 0x0128, 0x4000 },
    { 0x00BA, 0x4000 },
    { 0x00C6, 0x4000 },
    { 0x00D2, 0x4000 },
    { 0x00DE, 0x4000 },
    { 0x0132, 0x4000 },
  };

  GBA_System *system = GBA_GetSystem();
  *(system->interruptHandler) = GBA_InterruptHandler;

  GBA_InterruptStatus *status = system->interruptStatus;
  status->value = 0;

  GBA_InterruptControl *control = system->interruptControl;
  control->value |= (1 << interrupt);

  volatile void *base = system->displayControl;
  volatile u16 *sender = (u16 *) (base + senders[interrupt].offset);

  *sender |= senders[interrupt].flag;

  status->value = 1;
}

void
GBA_EnableMode(int mode) {
  GBA_System *system = GBA_GetSystem();

  u16 value = system->displayControl->value;
  value &= 0b1111111101111000;
  value |= mode;

  if (mode >= 3 && mode <= 5) {
    // enable BG2 for bitmap modes
    value &= 0b1111000011111111;
    value |= 0b0000010000000000;
  }

  system->displayControl->value = value;
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
  system->backgroundControls[layer]->value = control.value;
}

void
GBA_DisableBackgroundLayer(int layer) {
  GBA_System *system = GBA_GetSystem();

  u16 value = system->displayControl->value;
  value &= ~(1 << (8 + layer));
  value &= 0b1111111101111111; // unset force blank

  system->displayControl->value = value;
}

void
GBA_OffsetBackgroundLayer(
    int layer,
    int px, int py)
{
  GBA_System *system = GBA_GetSystem();

  system->backgroundOffsets[layer]->hOffset = px;
  system->backgroundOffsets[layer]->vOffset = py;
}

void
GBA_StartTimerCascade(
    GBA_TimerFrequency frequency,
    const GBA_TimerData *overflows)
{
  GBA_System *system = GBA_GetSystem();

  int count = 0;
  GBA_TimerControl timers[4] = {0};

  for (unsigned int index = 0; index < length(timers); index++) {
    GBA_TimerControl *timer = &timers[index];

    // prepare timer settings
    timer->enable = 1;
    timer->frequency = index > 0 ? 0 : frequency;
    timer->cascade = index > 0;

    // disable this timer first
    system->timerControl[index]->value = 0;

    if (count == 0 && overflows[index].value == 0) {
      count = index + 1;
    }
  }

  while (count > 0) {
    int index = --count;

    system->timerData[index]->value = overflows[index].value;
    system->timerControl[index]->value = timers[index].value;
  }
}

int
GBA_GetTimerValue(
    int index,
    const GBA_TimerData *overflows)
{
  GBA_System *system = GBA_GetSystem();

  int offset = 0xFFFF + overflows[index].value;
  int value = (system->timerData[index]->value - offset) & 0xFFFF;

  return value;
}

void
GBA_EnableSound() {
  GBA_System *system = GBA_GetSystem();
  GBA_SoundStatus *status = system->soundStatus;
  status->value = 1 << 7;
}

void
GBA_DisableSound() {
  GBA_System *system = GBA_GetSystem();
  GBA_SoundStatus *status = system->soundStatus;
  status->value = 0;
}

void
GBA_TileMapRef_FromBackgroundLayer(
    GBA_TileMapRef *tileMap,
    int layer)
{
  static const int widths[]  = { 32, 64, 32, 64 };
  static const int heights[] = { 32, 32, 64, 64 };

  GBA_System *system = GBA_GetSystem();
  GBA_BackgroundControl background = { system->backgroundControls[layer]->value };

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

  const int size = sizeof(GBA_TileMap) / sizeof(GBA_Tile);

  int index = (ty/32) * (tileMap->width/32) + (tx/32);
  int offset = index * size;

  return (ty%32)*32 + (tx%32) + offset;
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
GBA_TileMapRef_FillTile(
    GBA_TileMapRef *tileMap,
    int tileId, int color)
{
  GBA_Bitmap8 *bitmap = &tileMap->bitmaps[tileId];

  u8 pixel = color & 0xFF;
  u32 pixels = pixel << 24 | pixel << 16 | pixel << 8 | pixel;

  for (int i = 0; i < 16; i++) {
    bitmap->data[i] = pixels;
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
  GBA_System *system = GBA_GetSystem();

  while(system->vcount->value >= 160); // wait till VDraw
  while(system->vcount->value < 160);  // wait till VBlank
#endif
}

void
GBA_VSyncWait() {
#ifndef NOGBA
  asm volatile("swi 0x05");
#endif
}

#ifdef NOGBA

void
GBA_Memcpy32(void *dst, const void *src, int size) {
  unsigned char *target = dst;
  const unsigned char *source = src;

  for (int i = 0; i < size; i++) {
    target[i] = source[i];
  }
}

void
GBA_Memcpy16(void *dst, const void *src, int size) {
  return GBA_Memcpy32(dst, src, size);
}

void
GBA_Memset32(void *dst, int value, int size) {
  unsigned char *target = dst;
  unsigned char *source = (unsigned char *) &value;

  for (int i = 0; i < size; i++) {
    target[i] = source[i % sizeof(value)];
  }
}

void
GBA_Memset16(void *dst, short value, int size) {
  unsigned char *target = dst;
  unsigned char *source = (unsigned char *) &value;

  for (int i = 0; i < size; i++) {
    target[i] = source[i % sizeof(value)];
  }
}

#else

void
GBA_Memcpy32(void *dst, const void *src, int size) {
  GBA_System *system = GBA_GetSystem();
  GBA_DirectMemcpy *dma3 = system->directMemcpy[3];

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
GBA_Memcpy16(void *dst, const void *src, int size) {
  GBA_System *system = GBA_GetSystem();
  GBA_DirectMemcpy *dma3 = system->directMemcpy[3];

  GBA_DirectMemcpy copy = {0};
  copy.dst = dst;
  copy.src = src;
  copy.numTransfers = size >> 1;
  copy.chunkSize = 0; // copy half-words
  copy.enable = 1;

  dma3->dst = copy.dst;
  dma3->src = copy.src;
  dma3->cnt = copy.cnt;
}

void
GBA_Memset32(void *dst, int value, int size) {
  GBA_System *system = GBA_GetSystem();
  GBA_DirectMemcpy *dma3 = system->directMemcpy[3];

  GBA_DirectMemcpy copy = {0};
  copy.dst = dst;
  copy.src = &value;
  copy.numTransfers = size >> 2;
  copy.srcAdjust = 2; // fixed
  copy.chunkSize = 1; // copy words
  copy.enable = 1;

  dma3->dst = copy.dst;
  dma3->src = copy.src;
  dma3->cnt = copy.cnt;
}

void
GBA_Memset16(void *dst, short value, int size) {
  GBA_System *system = GBA_GetSystem();
  GBA_DirectMemcpy *dma3 = system->directMemcpy[3];

  GBA_DirectMemcpy copy = {0};
  copy.dst = dst;
  copy.src = &value;
  copy.numTransfers = size >> 1;
  copy.srcAdjust = 2; // fixed
  copy.chunkSize = 0; // copy half-words
  copy.enable = 1;

  dma3->dst = copy.dst;
  dma3->src = copy.src;
  dma3->cnt = copy.cnt;
}

#endif

void
GBA_EnableSprites() {
  GBA_System *system = GBA_GetSystem();

  GBA_DisplayControl control = { system->displayControl->value };
  control.sprite1DMapping = 1;
  control.enableOBJ = 1;

  system->displayControl->value = control.value;
}

static inline void
GBA_Sprite_Reset(GBA_Sprite *sprite) {
  static GBA_Sprite empty = { .objMode = GBA_SPRITE_MODE_HIDE };

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
    GBA_SpriteObjectMode mode)
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
#ifdef NOGBA
  const signed short sinlut[256] = {0};
#else
  extern const signed short sinlut[256];
#endif
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
GBA_Sprite_SetTileId(
    GBA_Sprite *sprite,
    int tileId)
{
  u16 attr2 = sprite->attr2 & ~(0b01111111111);
  attr2 |= tileId;
  sprite->attr2 = attr2;
}

void
GBA_Sprite_SetVFlip(
    GBA_Sprite *sprite,
    bool flip)
{
  u16 attr1 = sprite->attr1;

  if (flip) {
    attr1 |=  (1 << 13);
  } else {
    attr1 &= ~(1 << 13);
  }

  sprite->attr1 = attr1;
}

void
GBA_Sprite_SetHFlip(
    GBA_Sprite *sprite,
    bool flip)
{
  u16 attr1 = sprite->attr1;

  if (flip) {
    attr1 |=  (1 << 12);
  } else {
    attr1 &= ~(1 << 12);
  }

  sprite->attr1 = attr1;
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

int
GBA_Palette_FindColor(GBA_Color color) {
  GBA_System *system = GBA_GetSystem();
  GBA_Palette *palette = system->backgroundPalette;

  const int size = sizeof(GBA_Palette) / sizeof(GBA_Color);
  for (int index = 0; index < size; index++) {
    GBA_Color next = (*palette)[index];

    if (GBA_Color_Equals(next, color)) {
      return index;
    }
  }

  return -1;
}

int
GBA_Bitmap8_GetPixel(
    GBA_Bitmap8 *bitmap,
    int px, int py)
{
  px &= 0b111; // bitmaps are 8x8 pixels
  py &= 0b111;

  int offset = py * 8 + px;

  int index =  offset / 4; // a data point stores 4 pixel
  int shift = (offset % 4) * 8;

  return (bitmap->data[index] >> shift) & 0b11111111; // 8bpp
}

void
GBA_Bitmap_FillPixel(
    int px, int py,
    GBA_Color color)
{
  GBA_System *system = GBA_GetSystem();
  GBA_Color *pixels = system->pixels;

  const int height = GBA_SCREEN_HEIGHT;
  const int width = GBA_SCREEN_WIDTH;

  bool visible = true
    && px >= 0 && px < width
    && py >= 0 && py < height;

  if (visible) {
    int index = py * width + px;
    pixels[index].value = color.value;
  }
}

void
GBA_Bitmap_FillRect(
    int px, int py,
    int width, int height,
    GBA_Color color)
{
  for (int y = py; y < py + height; y++) {
    for (int x = px; x < px + width; x++) {
      GBA_Bitmap_FillPixel(x, y, color);
    }
  }
}

void
GBA_Bitmap_DrawRect(
    int px1, int py1,
    int px2, int py2,
    GBA_Color color)
{
  GBA_Bitmap_DrawLine(px1, py1, px2, py1, color);
  GBA_Bitmap_DrawLine(px1, py2, px2, py2, color);

  GBA_Bitmap_DrawLine(px1, py1, px1, py2, color);
  GBA_Bitmap_DrawLine(px2, py1, px2, py2, color);
}

void
GBA_Bitmap_DrawLine(
    int px1, int py1,
    int px2, int py2,
    GBA_Color color)
{
  int px = px1, py = py1;
  int dx = px2 - px1, dy = py2 - py1;
  int xstep = 1, ystep = 1;

  if (dx < 0) {
    dx = -dx;
    xstep = -xstep;
  }

  if (dy < 0) {
    dy = -dy;
    ystep = -ystep;
  }

  int xerror = 2 * dx;
  int yerror = 2 * dy;

  if (dy <= dx) {
    int error = -dx;

    do {
      GBA_Bitmap_FillPixel(px, py, color);

      error += yerror;
      if (error > 0) {
        py += ystep;
        error -= xerror;
      }

      px += xstep;
    } while (dx-- > 0);
  }

  else {
    int error = -dy;

    do {
      GBA_Bitmap_FillPixel(px, py, color);

      error += xerror;
      if (error > 0) {
        px += xstep;
        error -= yerror;
      }

      py += ystep;
    } while (dy-- > 0);
  }
}

void
GBA_Input_PollStates() {
  GBA_System *system = GBA_GetSystem();
  GBA_Input *input = &system->input;
  GBA_Keypad *keypad = system->keypad;

  input->previous.value = input->current.value;
  input->current.value = ~(keypad->value);
}

bool
GBA_Input_IsPressed(GBA_Key key) {
  GBA_System *system = GBA_GetSystem();
  GBA_Input *input = &system->input;
  return (input->current.value & key) > 0;
}

bool
GBA_Input_IsHit(GBA_Key key) {
  GBA_System *system = GBA_GetSystem();
  GBA_Input *input = &system->input;
  return ((input->current.value &~ input->previous.value) & key) > 0;
}

bool
GBA_Input_IsHeld(GBA_Key key) {
  GBA_System *system = GBA_GetSystem();
  GBA_Input *input = &system->input;
  return ((input->current.value & input->previous.value) & key) > 0;
}
