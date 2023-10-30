
#include <game/object.h>

typedef struct Properties {
  int width;
  int height;
  enum Variant {
    BOX_VARIANT_REGULAR,
    BOX_VARIANT_GRID,
  } variant;
} align4 Properties;

static inline bool
Object_CreateBox(
    Object *object,
    int width, int height,
    enum Variant variant)
{
  if (width < 1 || height < 1) {
    return false;
  }

  int x = width * 8;
  int y = height * 8;

  Bounds hitbox  = Bounds_Of(x, y, x, y);
  Bounds viewbox = Bounds_Of(x, y, x, y);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = false;
  object->type = TYPE_BOX;

  Properties *props = Object_GetProperties(object);
  props->width = width;
  props->height = height;
  props->variant = variant;

  return true;
}

bool
Object_CreateRegularBox(
    Object *object,
    int width, int height)
{
  return Object_CreateBox(object, width, height, BOX_VARIANT_REGULAR);
}

bool
Object_CreateGridBox(
    Object *object,
    int width, int height)
{
  return Object_CreateBox(object, width, height, BOX_VARIANT_GRID);
}

bool
Object_CreateBoxWithPole(
    Object *object,
    int height)
{
  if (height < 1) {
    return false;
  }

  int size = --height * 8;

  Bounds hitbox  = Bounds_Of(8, 32 + size, 8,  8 + size);
  Bounds viewbox = Bounds_Of(8, 20 + size, 8, 20 + size);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = false;
  object->type = TYPE_BOX_WITH_POLE;

  Properties *props = Object_GetProperties(object);
  props->height = height;

  return true;
}

bool
Object_CreateBoxWithChains(
    Object *object,
    int height)
{
  if (height < 1) {
    return false;
  }

  int size = --height * 8;

  Bounds hitbox  = Bounds_Of(8, 32 + size, 8,  8 + size);
  Bounds viewbox = Bounds_Of(8, 20 + size, 8, 20 + size);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = false;
  object->type = TYPE_BOX_WITH_CHAINS;

  Properties *props = Object_GetProperties(object);
  props->height = height;

  return true;
}

static const GBA_TileMapRef box = {
  .width = 2, .height = 2,
  .tiles = (GBA_Tile[]) {
    { .tileId = 35, .vFlip = 1, .hFlip = 0 },
    { .tileId = 35, .vFlip = 1, .hFlip = 1 },
    { .tileId = 35, .vFlip = 0, .hFlip = 0 },
    { .tileId = 35, .vFlip = 0, .hFlip = 1 },
  }
};

static void
Object_Draw1x1Box(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  GBA_TileMapRef_Blit(target, tx, ty, &box);
}

static const GBA_TileMapRef boxes[][3] = {
  { // top (left, middle, right)
    { .width = 2, .height = 2,
      .tiles = (GBA_Tile[]) {
        { .tileId = 35, .vFlip = 1, .hFlip = 0 },
        { .tileId = 36, .vFlip = 1, .hFlip = 0 },
        { .tileId = 27, .vFlip = 1, .hFlip = 0 },
        { .tileId = 28, .vFlip = 1, .hFlip = 0 },
      }
    },
    { .width = 2, .height = 2,
      .tiles = (GBA_Tile[]) {
        { .tileId = 36, .vFlip = 1, .hFlip = 1 },
        { .tileId = 36, .vFlip = 1, .hFlip = 0 },
        { .tileId = 28, .vFlip = 1, .hFlip = 1 },
        { .tileId = 28, .vFlip = 1, .hFlip = 0 },
      }
    },
    { .width = 2, .height = 2,
      .tiles = (GBA_Tile[]) {
        { .tileId = 36, .vFlip = 1, .hFlip = 1 },
        { .tileId = 35, .vFlip = 1, .hFlip = 1 },
        { .tileId = 28, .vFlip = 1, .hFlip = 1 },
        { .tileId = 27, .vFlip = 1, .hFlip = 1 },
      }
    }
  },
  { // center (left, middle, right)
    { .width = 2, .height = 2,
      .tiles = (GBA_Tile[]) {
        { .tileId = 27, .vFlip = 0, .hFlip = 0 },
        { .tileId = 28, .vFlip = 0, .hFlip = 0 },
        { .tileId = 27, .vFlip = 1, .hFlip = 0 },
        { .tileId = 28, .vFlip = 1, .hFlip = 0 },
      }
    },
    { .width = 2, .height = 2,
      .tiles = (GBA_Tile[]) {
        { .tileId = 28, .vFlip = 0, .hFlip = 1 },
        { .tileId = 28, .vFlip = 0, .hFlip = 0 },
        { .tileId = 28, .vFlip = 1, .hFlip = 1 },
        { .tileId = 28, .vFlip = 1, .hFlip = 0 },
      }
    },
    { .width = 2, .height = 2,
      .tiles = (GBA_Tile[]) {
        { .tileId = 28, .vFlip = 0, .hFlip = 1 },
        { .tileId = 27, .vFlip = 0, .hFlip = 1 },
        { .tileId = 28, .vFlip = 1, .hFlip = 1 },
        { .tileId = 27, .vFlip = 1, .hFlip = 1 },
      }
    }
  },
  { // bottom (left, middle, right)
    { .width = 2, .height = 2,
      .tiles = (GBA_Tile[]) {
        { .tileId = 27, .vFlip = 0, .hFlip = 0 },
        { .tileId = 28, .vFlip = 0, .hFlip = 0 },
        { .tileId = 35, .vFlip = 0, .hFlip = 0 },
        { .tileId = 36, .vFlip = 0, .hFlip = 0 },
      }
    },
    { .width = 2, .height = 2,
      .tiles = (GBA_Tile[]) {
        { .tileId = 28, .vFlip = 0, .hFlip = 1 },
        { .tileId = 28, .vFlip = 0, .hFlip = 0 },
        { .tileId = 36, .vFlip = 0, .hFlip = 1 },
        { .tileId = 36, .vFlip = 0, .hFlip = 0 },
      }
    },
    { .width = 2, .height = 2,
      .tiles = (GBA_Tile[]) {
        { .tileId = 28, .vFlip = 0, .hFlip = 1 },
        { .tileId = 27, .vFlip = 0, .hFlip = 1 },
        { .tileId = 36, .vFlip = 0, .hFlip = 1 },
        { .tileId = 35, .vFlip = 0, .hFlip = 1 },
      }
    }
  },
};

static const GBA_TileMapRef block = {
  .width = 2, .height = 2,
  .tiles = (GBA_Tile[]) {
    { .tileId = 18, .vFlip = 0, .hFlip = 0 },
    { .tileId = 18, .vFlip = 0, .hFlip = 1 },
    { .tileId = 26, .vFlip = 0, .hFlip = 0 },
    { .tileId = 26, .vFlip = 0, .hFlip = 1 },
  }
};

static void
Object_DrawRegularBoxes(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector lower = Bounds_Lower(&object->viewbox);
  Vector upper = Bounds_Upper(&object->viewbox);

  Vector_Rshift(&lower, 3);
  Vector_Rshift(&upper, 3);

  for (int y = lower.y; y < upper.y; y += 2) {
    for (int x = lower.x; x < upper.x; x += 2) {
      GBA_TileMapRef_Blit(target, x, y, &block);
    }
  }
}

static void
Object_DrawGridBoxes(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector lower = Bounds_Lower(&object->viewbox);
  Vector upper = Bounds_Upper(&object->viewbox);

  Vector_Rshift(&lower, 3);
  Vector_Rshift(&upper, 3);

  for (int y = lower.y; y < upper.y; y += 2) {
    int row = (y != lower.y) + (y == upper.y-2);

    for (int x = lower.x; x < upper.x; x += 2) {
      int col = (x != lower.x) + (x == upper.x-2);

      const GBA_TileMapRef *tiles = &boxes[row][col];
      GBA_TileMapRef_Blit(target, x, y, tiles);
    }
  }
}

static const GBA_TileMapRef vbox[] = {
  // top
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 35, .vFlip = 1, .hFlip = 0 },
      { .tileId = 35, .vFlip = 1, .hFlip = 1 },
      { .tileId = 27, .vFlip = 1, .hFlip = 0 },
      { .tileId = 27, .vFlip = 1, .hFlip = 1 },
    }
  },
  // middle
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 27, .vFlip = 0, .hFlip = 0 },
      { .tileId = 27, .vFlip = 0, .hFlip = 1 },
      { .tileId = 27, .vFlip = 1, .hFlip = 0 },
      { .tileId = 27, .vFlip = 1, .hFlip = 1 },
    }
  },
  // bottom
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 27, .vFlip = 0, .hFlip = 0 },
      { .tileId = 27, .vFlip = 0, .hFlip = 1 },
      { .tileId = 35, .vFlip = 0, .hFlip = 0 },
      { .tileId = 35, .vFlip = 0, .hFlip = 1 },
    }
  }
};

static void
Object_DrawVbox(
    Object *object,
    GBA_TileMapRef *target,
    int height)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  for (int y = 1; y < height; y++) {
    const GBA_TileMapRef *tiles = &vbox[y != 1];
    GBA_TileMapRef_Blit(target, tx, ty, tiles);
    ty += tiles->height;
  }

  GBA_TileMapRef_Blit(target, tx, ty, &vbox[2]);
}

static const GBA_TileMapRef hbox[] = {
  // left
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 35, .vFlip = 1, .hFlip = 0 },
      { .tileId = 36, .vFlip = 1, .hFlip = 0 },
      { .tileId = 35, .vFlip = 0, .hFlip = 0 },
      { .tileId = 36, .vFlip = 0, .hFlip = 0 },
    }
  },
  // middle
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 36, .vFlip = 1, .hFlip = 0 },
      { .tileId = 36, .vFlip = 1, .hFlip = 1 },
      { .tileId = 36, .vFlip = 0, .hFlip = 0 },
      { .tileId = 36, .vFlip = 0, .hFlip = 1 },
    }
  },
  // right
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 36, .vFlip = 1, .hFlip = 0 },
      { .tileId = 35, .vFlip = 1, .hFlip = 1 },
      { .tileId = 36, .vFlip = 0, .hFlip = 0 },
      { .tileId = 35, .vFlip = 0, .hFlip = 1 },
    }
  }
};

static void
Object_DrawHbox(
    Object *object,
    GBA_TileMapRef *target,
    int width)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  for (int x = 1; x < width; x++) {
    const GBA_TileMapRef *tiles = &hbox[x != 1];
    GBA_TileMapRef_Blit(target, tx, ty, tiles);
    tx += tiles->width;
  }

  GBA_TileMapRef_Blit(target, tx, ty, &hbox[2]);
}

void
Object_DrawBox(
    Object *object,
    GBA_TileMapRef *target)
{
  Properties *props = Object_GetProperties(object);
  int width = props->width;
  int height = props->height;

  enum Variant variant = props->variant;
  if (variant == BOX_VARIANT_REGULAR) {
    return Object_DrawRegularBoxes(object, target);
  }

  int size = width * height;
  if (size == 1) {
    Object_Draw1x1Box(object, target);
  } else if (size == height) {
    Object_DrawVbox(object, target, height);
  } else if (size == width) {
    Object_DrawHbox(object, target, width);
  } else {
    Object_DrawGridBoxes(object, target);
  }
}

static const GBA_TileMapRef pole[] = {
  // pole
  { .width = 2, .height = 5,
    .tiles = (GBA_Tile[]) {
      { .tileId = 22, .vFlip = 0, .hFlip = 0 },
      { .tileId = 22, .vFlip = 0, .hFlip = 1 },
      { .tileId = 30, .vFlip = 0, .hFlip = 0 },
      { .tileId = 31, .vFlip = 0, .hFlip = 0 },
      { .tileId = 38, .vFlip = 0, .hFlip = 0 },
      { .tileId = 39, .vFlip = 0, .hFlip = 0 },
      // block
      { .tileId = 18, .vFlip = 0, .hFlip = 0 },
      { .tileId = 18, .vFlip = 0, .hFlip = 1 },
      { .tileId = 26, .vFlip = 0, .hFlip = 0 },
      { .tileId = 26, .vFlip = 0, .hFlip = 1 },
    }
  },
  // pole frames
  { .width = 2, .height = 1,
    .tiles = (GBA_Tile[]) {
      { .tileId = 22, .vFlip = 0, .hFlip = 0 },
      { .tileId = 22, .vFlip = 0, .hFlip = 1 },
    }
  },
  { .width = 2, .height = 1,
    .tiles = (GBA_Tile[]) {
      { .tileId = 22, .vFlip = 0, .hFlip = 0 },
      { .tileId = 22, .vFlip = 0, .hFlip = 1 },
    }
  },
  { .width = 2, .height = 1,
    .tiles = (GBA_Tile[]) {
      { .tileId = 22, .vFlip = 0, .hFlip = 0 },
      { .tileId = 22, .vFlip = 0, .hFlip = 1 },
    }
  },
  { .width = 2, .height = 1,
    .tiles = (GBA_Tile[]) {
      { .tileId = 22, .vFlip = 0, .hFlip = 0 },
      { .tileId = 22, .vFlip = 0, .hFlip = 1 },
    }
  },
  { .width = 2, .height = 1,
    .tiles = (GBA_Tile[]) {
      { .tileId = 23, .vFlip = 0, .hFlip = 0 },
      { .tileId = 23, .vFlip = 0, .hFlip = 1 },
    }
  },
  { .width = 2, .height = 1,
    .tiles = (GBA_Tile[]) {
      { .tileId = 47, .vFlip = 0, .hFlip = 0 },
      { .tileId = 47, .vFlip = 0, .hFlip = 1 },
    }
  },
  { .width = 2, .height = 1,
    .tiles = (GBA_Tile[]) {
      { .tileId = 23, .vFlip = 0, .hFlip = 0 },
      { .tileId = 23, .vFlip = 0, .hFlip = 1 },
    }
  },
  { .width = 2, .height = 1,
    .tiles = (GBA_Tile[]) {
      { .tileId = 22, .vFlip = 0, .hFlip = 0 },
      { .tileId = 22, .vFlip = 0, .hFlip = 1 },
    }
  }
};

void
Object_DrawBoxWithPole(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  const GBA_TileMapRef *tiles = &pole[0];
  GBA_TileMapRef_Blit(target, tx, ty, tiles);
  ty += tiles->height;

  Properties *props = Object_GetProperties(object);
  int height = props->height;

  if (height-- > 0) {
    for (int y = 0; y < height; y++) {
      const GBA_TileMapRef *tiles = &vbox[y != 0];
      GBA_TileMapRef_Blit(target, tx, ty, tiles);
      ty += tiles->height;
    }

    GBA_TileMapRef_Blit(target, tx, ty, &vbox[2]);
  }
}

bool
Object_AnimateBoxWithPole(
    Object *object,
    GBA_TileMapRef *target,
    int frame)
{
  int index = Math_mod2(frame / 4, 3);

  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  const GBA_TileMapRef *tiles = &pole[1 + index];
  GBA_TileMapRef_Blit(target, tx, ty, tiles);

  return true;
}

static const GBA_TileMapRef chains = {
  .width = 2, .height = 5,
  .tiles = (GBA_Tile[]) {
    // chains
    { .tileId = 88, .vFlip = 0, .hFlip = 0 },
    { .tileId = 89, .vFlip = 0, .hFlip = 0 },
    { .tileId = 90, .vFlip = 0, .hFlip = 0 },
    { .tileId = 91, .vFlip = 0, .hFlip = 0 },
    { .tileId = 92, .vFlip = 0, .hFlip = 0 },
    { .tileId = 93, .vFlip = 0, .hFlip = 0 },
    // block
    { .tileId = 18, .vFlip = 0, .hFlip = 0 },
    { .tileId = 18, .vFlip = 0, .hFlip = 1 },
    { .tileId = 26, .vFlip = 0, .hFlip = 0 },
    { .tileId = 26, .vFlip = 0, .hFlip = 1 },
  }
};

void
Object_DrawBoxWithChains(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  const GBA_TileMapRef *tiles = &chains;
  GBA_TileMapRef_Blit(target, tx, ty, tiles);
  ty += tiles->height;

  Properties *props = Object_GetProperties(object);
  int height = props->height;

  if (height > 0) {
    const GBA_TileMapRef tiles = {
      .width = 2, .height = 2,
      .tiles = &chains.tiles[6],
    };

    for (int y = 0; y < height; y++) {
      GBA_TileMapRef_Blit(target, tx, ty, &tiles);
      ty += tiles.height;
    }
  }
}
