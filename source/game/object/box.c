
#include <game/object.h>

typedef struct Properties {
  int width;
  int height;
  enum packed {
    BOX_VARIANT_REGULAR = 1,
    BOX_VARIANT_GRID = 2,
  } variant;
  struct Faces faces[16];
} align4 Properties;

static inline bool
Object_CreateBox(
    Object *object,
    Faces *faces,
    int width, int height,
    int variant)
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
  object->type = OBJECT_TYPE_BOX;

  Properties *props = Object_GetProperties(object);
  props->width = width;
  props->height = height;
  props->variant = variant;

  if (faces != NULL) {
    int index = 0;
    int size = width * height;

    while (size-- > 0) {
      props->faces[index] = faces[index];
      size -= faces[index].repeat;
      index++;
    }
  }

  return true;
}

bool
Object_CreateRegularBox(
    Object *object,
    int width, int height)
{
  return Object_CreateBox(object, NULL, width, height, BOX_VARIANT_REGULAR);
}

bool
Object_CreateGridBox(
    Object *object,
    Faces *faces,
    int width, int height)
{
  return Object_CreateBox(object, faces, width, height, BOX_VARIANT_GRID);
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
  object->type = OBJECT_TYPE_BOX_WITH_POLE;

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
  object->type = OBJECT_TYPE_BOX_WITH_CHAINS;

  Properties *props = Object_GetProperties(object);
  props->height = height;

  return true;
}

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

static const GBA_TileMapRef boxes[] = {
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
      { .tileId = 27, .vFlip = 0, .hFlip = 0 },
      { .tileId = 28, .vFlip = 0, .hFlip = 0 },
      { .tileId = 27, .vFlip = 1, .hFlip = 0 },
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
  },
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 27, .vFlip = 0, .hFlip = 0 },
      { .tileId = 27, .vFlip = 0, .hFlip = 1 },
      { .tileId = 27, .vFlip = 1, .hFlip = 0 },
      { .tileId = 27, .vFlip = 1, .hFlip = 1 },
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
      { .tileId = 35, .vFlip = 1, .hFlip = 0 },
      { .tileId = 36, .vFlip = 1, .hFlip = 0 },
      { .tileId = 27, .vFlip = 1, .hFlip = 0 },
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
  },
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 35, .vFlip = 1, .hFlip = 0 },
      { .tileId = 35, .vFlip = 1, .hFlip = 1 },
      { .tileId = 27, .vFlip = 1, .hFlip = 0 },
      { .tileId = 27, .vFlip = 1, .hFlip = 1 },
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
      { .tileId = 27, .vFlip = 0, .hFlip = 0 },
      { .tileId = 28, .vFlip = 0, .hFlip = 0 },
      { .tileId = 35, .vFlip = 0, .hFlip = 0 },
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
  },
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 27, .vFlip = 0, .hFlip = 0 },
      { .tileId = 27, .vFlip = 0, .hFlip = 1 },
      { .tileId = 35, .vFlip = 0, .hFlip = 0 },
      { .tileId = 35, .vFlip = 0, .hFlip = 1 },
    }
  },
  { .width = 2,  .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 36, .vFlip = 1, .hFlip = 1 },
      { .tileId = 36, .vFlip = 1, .hFlip = 0 },
      { .tileId = 36, .vFlip = 0, .hFlip = 1 },
      { .tileId = 36, .vFlip = 0, .hFlip = 0 },
    }
  },
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 35, .vFlip = 1, .hFlip = 0 },
      { .tileId = 36, .vFlip = 1, .hFlip = 0 },
      { .tileId = 35, .vFlip = 0, .hFlip = 0 },
      { .tileId = 36, .vFlip = 0, .hFlip = 0 },
    }
  },
  { .width = 2,  .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 36, .vFlip = 1, .hFlip = 1 },
      { .tileId = 35, .vFlip = 1, .hFlip = 1 },
      { .tileId = 36, .vFlip = 0, .hFlip = 1 },
      { .tileId = 35, .vFlip = 0, .hFlip = 1 },
    }
  },
  { .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 35, .vFlip = 1, .hFlip = 0 },
      { .tileId = 35, .vFlip = 1, .hFlip = 1 },
      { .tileId = 35, .vFlip = 0, .hFlip = 0 },
      { .tileId = 35, .vFlip = 0, .hFlip = 1 },
    }
  }
};

static void
Object_DrawGridBoxes(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector lower = Bounds_Lower(&object->viewbox);
  Vector upper = Bounds_Upper(&object->viewbox);

  Vector_Rshift(&lower, 3);
  Vector_Rshift(&upper, 3);

  Properties *props = Object_GetProperties(object);

  int index = 0;
  Faces faces = props->faces[index];

  for (int y = lower.y; y < upper.y; y += 2) {
    for (int x = lower.x; x < upper.x; x += 2) {
      const GBA_TileMapRef *tiles = &boxes[faces.flags];
      GBA_TileMapRef_Blit(target, x, y, tiles);

      if (faces.repeat > 0) {
        faces.repeat--;
        continue;
      }

      faces = props->faces[++index];
    }
  }
}

void
Object_DrawBox(
    Object *object,
    GBA_TileMapRef *target)
{
  Properties *props = Object_GetProperties(object);

  int variant = props->variant;
  if (variant == BOX_VARIANT_REGULAR) {
    Object_DrawRegularBoxes(object, target);
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
