
#include <game/object.h>

typedef struct Properties {
  Direction direction;
  Vector vertices[3];
  enum Variant {
    SPIKE_VARIANT_REGULAR = 0,
    SPIKE_VARIANT_TINY = 4,
  } variant;
} align4 Properties;

static const Vector vertices[][3] = {
  [SPIKE_VARIANT_REGULAR + DIRECTION_LEFT] = {
    Vector_Of(0, 8),
    Vector_Of(16, 0),
    Vector_Of(16, 16),
  },
  [SPIKE_VARIANT_REGULAR + DIRECTION_RIGHT] = {
    Vector_Of(0, 0),
    Vector_Of(0, 16),
    Vector_Of(16, 8),
  },
  [SPIKE_VARIANT_REGULAR + DIRECTION_UP] = {
    Vector_Of(8, 0),
    Vector_Of(0, 16),
    Vector_Of(16, 16),
  },
  [SPIKE_VARIANT_REGULAR + DIRECTION_DOWN] = {
    Vector_Of(0, 0),
    Vector_Of(8, 16),
    Vector_Of(16, 0),
  },
  [SPIKE_VARIANT_TINY] = {
    Vector_Of(1, 8),
    Vector_Of(8, 0),
    Vector_Of(15, 8),
  }
};

bool
Object_CreateSpike(
    Object *object,
    Direction direction)
{
  const enum Variant variant = SPIKE_VARIANT_REGULAR;

  Bounds hitbox  = Bounds_Of(8, 8, 8, 8);
  Bounds viewbox = Bounds_Of(8, 8, 8, 8);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = true;
  object->type = TYPE_SPIKE;

  Properties *props = Object_GetProperties(object);
  props->direction = direction;
  props->variant = variant;

  for (unsigned int i = 0; i < length(props->vertices); i++) {
    props->vertices[i] = vertices[variant + direction][i];
  }

  return true;
}

bool
Object_CreateTinySpike(Object *object) {
  const enum Variant variant = SPIKE_VARIANT_TINY;

  Bounds hitbox  = Bounds_Of(8, 4, 8, 4);
  Bounds viewbox = Bounds_Of(8, 4, 8, 4);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = true;
  object->deadly = true;
  object->type = TYPE_SPIKE;

  Properties *props = Object_GetProperties(object);
  props->direction = 0;
  props->variant = variant;

  for (unsigned int i = 0; i < length(props->vertices); i++) {
    props->vertices[i] = vertices[variant][i];
  }

  return true;
}

static const GBA_TileMapRef spikes[] = {
  [SPIKE_VARIANT_REGULAR + DIRECTION_LEFT] = {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 43, .vFlip = 1, .hFlip = 0 },
      { .tileId = 44, .vFlip = 1, .hFlip = 0 },
      { .tileId = 43, .vFlip = 0, .hFlip = 0 },
      { .tileId = 44, .vFlip = 0, .hFlip = 0 },
    }
  },
  [SPIKE_VARIANT_REGULAR + DIRECTION_RIGHT] = {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 44, .vFlip = 1, .hFlip = 1 },
      { .tileId = 43, .vFlip = 1, .hFlip = 1 },
      { .tileId = 44, .vFlip = 0, .hFlip = 1 },
      { .tileId = 43, .vFlip = 0, .hFlip = 1 },
    }
  },
  [SPIKE_VARIANT_REGULAR + DIRECTION_UP] = {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 17, .vFlip = 0, .hFlip = 0 },
      { .tileId = 17, .vFlip = 0, .hFlip = 1 },
      { .tileId = 25, .vFlip = 0, .hFlip = 0 },
      { .tileId = 25, .vFlip = 0, .hFlip = 1 },
    }
  },
  [SPIKE_VARIANT_REGULAR + DIRECTION_DOWN] = {
    .width = 2, .height = 2,
    .tiles = (GBA_Tile[]) {
      { .tileId = 25, .vFlip = 1, .hFlip = 0 },
      { .tileId = 25, .vFlip = 1, .hFlip = 1 },
      { .tileId = 17, .vFlip = 1, .hFlip = 0 },
      { .tileId = 17, .vFlip = 1, .hFlip = 1 },
    }
  },
  [SPIKE_VARIANT_TINY] = {
    .width = 2, .height = 1,
    .tiles = (GBA_Tile[]) {
      { .tileId = 45, .vFlip = 0, .hFlip = 0 },
      { .tileId = 45, .vFlip = 0, .hFlip = 1 },
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

  for (unsigned int i = 0; i < length(props->vertices); i++) {
    props->vertices[i].x += dx;
    props->vertices[i].y += dy;
  }
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
  enum Variant variant = props->variant;

  const GBA_TileMapRef *tiles = &spikes[variant + direction];
  GBA_TileMapRef_Blit(target, tx, ty, tiles);
}
