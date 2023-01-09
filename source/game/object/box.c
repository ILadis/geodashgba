
#include <game/object.h>

typedef struct Properties {
  int width;
  int height;
} align4 Properties;

bool
Object_CreateBox(
    Object *object,
    int width, int height)
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

static void
Object_DrawBoxes(
    Object *object,
    GBA_TileMapRef *target,
    int width, int height)
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

  int size = width * height;

  if (size == 1) {
    Object_Draw1x1Box(object, target);
  } else if (size == height) {
    Object_DrawVbox(object, target, height);
  } else if (size == width) {
    Object_DrawHbox(object, target, width);
  } else {
    Object_DrawBoxes(object, target, width, height);
  }
}
