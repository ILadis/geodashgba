
#include <game/object.h>

typedef struct Properties {
  Direction direction;
  Vector vertices[3];
} align4 Properties;

static const Vector vertices[][3] = {
  [DIRECTION_LEFT] = {
    Vector_Of(0, 8),
    Vector_Of(16, 0),
    Vector_Of(16, 16),
  },
  [DIRECTION_RIGHT] = {
    Vector_Of(0, 0),
    Vector_Of(0, 16),
    Vector_Of(16, 8),
  },
  [DIRECTION_UP] = {
    Vector_Of(8, 0),
    Vector_Of(0, 16),
    Vector_Of(16, 16),
  },
  [DIRECTION_DOWN] = {
    Vector_Of(0, 0),
    Vector_Of(8, 16),
    Vector_Of(16, 0),
  }
};

bool
Object_CreateSpike(
    Object *object,
    Direction direction)
{
  Bounds hitbox  = Bounds_Of(8, 8, 8, 8);
  Bounds viewbox = Bounds_Of(8, 8, 8, 8);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = true;
  object->type = TYPE_SPIKE;

  Properties *props = Object_GetProperties(object);
  props->direction = direction;

  for (int i = 0; i < length(props->vertices); i++) {
    props->vertices[i] = vertices[direction][i];
  }

  return true;
}

static const GBA_TileMapRef spikes[] = {
  [DIRECTION_LEFT] = {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 43, .vFlip = 1, .hFlip = 0 },
      { .tileId = 44, .vFlip = 1, .hFlip = 0 },
      { .tileId = 43, .vFlip = 0, .hFlip = 0 },
      { .tileId = 44, .vFlip = 0, .hFlip = 0 },
    }
  },
  [DIRECTION_RIGHT] = {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 44, .vFlip = 1, .hFlip = 1 },
      { .tileId = 43, .vFlip = 1, .hFlip = 1 },
      { .tileId = 44, .vFlip = 0, .hFlip = 1 },
      { .tileId = 43, .vFlip = 0, .hFlip = 1 },
    }
  },
  [DIRECTION_UP] = {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 17, .vFlip = 0, .hFlip = 0 },
      { .tileId = 17, .vFlip = 0, .hFlip = 1 },
      { .tileId = 25, .vFlip = 0, .hFlip = 0 },
      { .tileId = 25, .vFlip = 0, .hFlip = 1 },
    }
  },
  [DIRECTION_DOWN] = {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 25, .vFlip = 1, .hFlip = 0 },
      { .tileId = 25, .vFlip = 1, .hFlip = 1 },
      { .tileId = 17, .vFlip = 1, .hFlip = 0 },
      { .tileId = 17, .vFlip = 1, .hFlip = 1 },
    }
  }
};

bool
Object_HitSpike(
    Object *object,
    Shape *shape)
{
  Properties *props = Object_GetProperties(object);
  Shape hitbox = Shape_Of(props->vertices);

  return Shape_Intersects(&hitbox, shape);
}

void
Object_MoveSpike(
    Object *object,
    Vector *position)
{
  Properties *props = Object_GetProperties(object);

  int dx = position->x * 8;
  int dy = position->y * 8;

  for (int i = 0; i < length(props->vertices); i++) {
    props->vertices[i].x += dx;
    props->vertices[i].y += dy;
  }

  dx++;
}

void
Object_DrawSpike(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  Properties *props = Object_GetProperties(object);
  Direction direction = props->direction;

  const GBA_TileMapRef *tiles = &spikes[direction];
  GBA_TileMapRef_Blit(target, tx, ty, tiles);
}
