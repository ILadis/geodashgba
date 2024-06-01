
#include <game/object.h>
#include <game/cube.h>

typedef struct Properties {
  bool triggered;
  // TODO adjustable launch height
} align4 Properties;

bool
Object_CreatePad(Object *object) {
  Bounds hitbox  = Bounds_Of(8, 4, 4, 4);
  Bounds viewbox = Bounds_Of(8, 0, 8, 8);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = false;
  object->deadly = false;
  object->type = OBJECT_TYPE_PAD;

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

  // TODO workaround to prevent shadow from triggering pads
  if (Cube_IsShadowShape(shape)) {
    return true;
  }

  props->triggered = true;

  Cube *cube = Cube_GetInstance();
  Cube_Launch(cube, 1800);

  return true;
}

static const GBA_TileMapRef pad[] = {
  // pad
  { .width = 2, .height = 2,
    .tiles = GBA_Tiles_Of(
      { .tileId = 112, .vFlip = 0, .hFlip = 0 },
      { .tileId = 114, .vFlip = 0, .hFlip = 1 },
      { .tileId = 120, .vFlip = 0, .hFlip = 0 },
      { .tileId = 122, .vFlip = 0, .hFlip = 1 },
    )
  },
  // pad frames
  { .width = 2, .height = 2,
    .tiles = GBA_Tiles_Of(
      { .tileId = 113, .vFlip = 0, .hFlip = 0 },
      { .tileId = 115, .vFlip = 0, .hFlip = 1 },
      { .tileId = 121, .vFlip = 0, .hFlip = 0 },
      { .tileId = 123, .vFlip = 0, .hFlip = 1 },
    )
  },
  { .width = 2, .height = 2,
    .tiles = GBA_Tiles_Of(
      { .tileId = 114, .vFlip = 0, .hFlip = 0 },
      { .tileId = 112, .vFlip = 0, .hFlip = 1 },
      { .tileId = 122, .vFlip = 0, .hFlip = 0 },
      { .tileId = 120, .vFlip = 0, .hFlip = 1 },
    )
  },
  { .width = 2, .height = 2,
    .tiles = GBA_Tiles_Of(
      { .tileId = 115, .vFlip = 0, .hFlip = 0 },
      { .tileId = 113, .vFlip = 0, .hFlip = 1 },
      { .tileId = 123, .vFlip = 0, .hFlip = 0 },
      { .tileId = 121, .vFlip = 0, .hFlip = 1 },
    )
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

  GBA_TileMapRef_Blit(target, tx, ty, &pad[0]);
}

bool
Object_AnimatePad(
    Object *object,
    GBA_TileMapRef *target,
    int frame)
{
  int index = Math_mod2(frame >> 3, 2);

  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  const GBA_TileMapRef *tiles = &pad[index];
  GBA_TileMapRef_Blit(target, tx, ty, tiles);

  return true;
}
