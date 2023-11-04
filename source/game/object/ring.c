
#include <game/object.h>
#include <game/cube.h>

typedef struct Properties {
  bool triggered;
  // TODO adjustable launch height
} align4 Properties;

bool
Object_CreateRing(Object *object) {
  Bounds hitbox  = Bounds_Of(8, 8, 14, 14);
  Bounds viewbox = Bounds_Of(8, 8, 8, 8);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = false;
  object->deadly = false;
  object->type = TYPE_RING;

  Properties *props = Object_GetProperties(object);
  props->triggered = false;

  return true;
}

bool
Object_HitRing(
    Object *object,
    Shape *shape)
{
  Properties *props = Object_GetProperties(object);

  bool triggered = props->triggered;
  if (triggered) {
    return true;
  }

  // TODO workaround to prevent shadow from triggering rings
  if (Cube_IsShadowShape(shape)) {
    return true;
  }

  if (GBA_Input_IsHit(GBA_KEY_A)) {
    props->triggered = true;

    Cube *cube = Cube_GetInstance();
    Cube_Launch(cube, 1400);
  }

  return true;
}

static const GBA_TileMapRef ring = {
  .width = 2, .height = 2,
  .tiles = (GBA_Tile[]) {
    { .tileId = 46, .vFlip = 0, .hFlip = 0 },
    { .tileId = 46, .vFlip = 0, .hFlip = 1 },
    { .tileId = 46, .vFlip = 1, .hFlip = 0 },
    { .tileId = 46, .vFlip = 1, .hFlip = 1 },
  }
};

void
Object_DrawRing(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  GBA_TileMapRef_Blit(target, tx, ty, &ring);
}
