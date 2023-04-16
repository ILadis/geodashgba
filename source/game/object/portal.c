
#include <game/object.h>
#include <game/cube.h>

typedef struct Properties {
  bool triggered;
} align4 Properties;

bool
Object_CreatePortal(Object *object) {
  Bounds hitbox  = Bounds_Of(8, 16, 8, 14);
  Bounds viewbox = Bounds_Of(8, 16, 8, 16);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = false;
  object->deadly = false;
  object->type = TYPE_PORTAL;

  Properties *props = Object_GetProperties(object);
  props->triggered = false;

  return true;
}

bool
Object_HitPortal(
    Object *object,
    Shape *shape)
{
  static Dynamics dynamics = {0};
  Properties *props = Object_GetProperties(object);

  bool triggered = props->triggered;
  if (triggered) {
    return true;
  }

  Cube *cube = Cube_GetInstance();
  // TODO workaround to prevent shadow from triggering portals
  if (&cube->shape != shape) {
    return true;
  }

  props->triggered = true;

  Body *body = &cube->body;
  dynamics = Dynamics_OfInverseGravity(body->dynamics);

  Body_SetDynamics(body, &dynamics);

  return true;
}

static const GBA_TileMapRef portal = {
  .width = 2, .height = 4,
  .tiles = (GBA_Tile[]) {
    { .tileId = 168, .vFlip = 0, .hFlip = 0 },
    { .tileId = 169, .vFlip = 0, .hFlip = 0 },
    { .tileId = 170, .vFlip = 0, .hFlip = 0 },
    { .tileId = 171, .vFlip = 0, .hFlip = 0 },
    { .tileId = 172, .vFlip = 0, .hFlip = 0 },
    { .tileId = 173, .vFlip = 0, .hFlip = 0 },
    { .tileId = 174, .vFlip = 0, .hFlip = 0 },
    { .tileId = 175, .vFlip = 0, .hFlip = 0 },
  }
};

void
Object_DrawPortal(
    Object *object,
    GBA_TileMapRef *target)
{
  Vector position = Bounds_Lower(&object->viewbox);

  int tx = position.x / 8;
  int ty = position.y / 8;

  GBA_TileMapRef_Blit(target, tx, ty, &portal);
}
