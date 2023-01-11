
#include <game/object.h>
#include <game/cube.h>

typedef struct Properties {
  bool triggered;
  // TODO adjustable launch height
} align4 Properties;

bool
Object_CreatePad(Object *object) {
  Bounds hitbox  = Bounds_Of(8, 4, 4, 4);
  Bounds viewbox = Bounds_Of(8, 4, 8, 4);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = false;
  object->deadly = false;
  object->type = TYPE_PAD;

  Properties *props = Object_GetProperties(object);
  props->triggered = false;

  return true;
}

bool
Object_HitPad(
    Object *object,
    Shape *shape)
{
  Properties *props = Object_GetProperties(object);

  bool triggered = props->triggered;
  if (triggered) {
    return true;
  }

  Cube *cube = Cube_GetInstance();
  // TODO workaround to prevent shadow from triggering pads
  if (&cube->shape != shape) {
    return true;
  }

  props->triggered = true;
  Cube_Launch(cube, 1800);

  return true;
}

static const GBA_TileMapRef pad = {
  .width = 2, .height = 1,
  .tiles = (GBA_Tile[]) {
    { .tileId = 3, .vFlip = 0, .hFlip = 0 },
    { .tileId = 3, .vFlip = 0, .hFlip = 1 },
  }
};

void
Object_DrawPad(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  GBA_TileMapRef_Blit(target, tx, ty, &pad);
}
