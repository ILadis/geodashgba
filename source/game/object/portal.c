
#include <game/object.h>
#include <game/cube.h>

typedef struct Properties {
  bool triggered;
  enum packed {
    PORTAL_VARIANT_FLIP_GRAVITY = 1,
    PORTAL_VARIANT_TOGGLE_FLY = 2,
  } variant;
} align4 Properties;

static inline bool
Object_CreatePortal(
    Object *object,
    int variant)
{
  Bounds hitbox  = Bounds_Of(12, 24, 12, 24);
  Bounds viewbox = Bounds_Of(12, 24, 12, 24);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = false;
  object->deadly = false;
  object->type = OBJECT_TYPE_PORTAL;

  Properties *props = Object_GetProperties(object);
  props->triggered = false;
  props->variant = variant;

  return true;
}

bool
Object_CreateGravityPortal(Object *object) {
  return Object_CreatePortal(object, PORTAL_VARIANT_FLIP_GRAVITY);
}

bool
Object_CreateFlyPortal(Object *object) {
  return Object_CreatePortal(object, PORTAL_VARIANT_TOGGLE_FLY);
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

  // TODO workaround to prevent shadow from triggering portals
  if (Cube_IsShadowShape(shape)) {
    return true;
  }

  props->triggered = true;

  Cube *cube = Cube_GetInstance();

  switch (props->variant) {
  case PORTAL_VARIANT_FLIP_GRAVITY:
    const Body *body = Cube_GetBody(cube);
    dynamics = Dynamics_OfInverseGravity(body->dynamics);
    Cube_ChangeDynamics(cube, &dynamics);
    break;
  case PORTAL_VARIANT_TOGGLE_FLY:
    if (Cube_IsTraitEnabled(cube, TRAIT_TYPE_FLY)) {
      Cube_SetTraitEnabled(cube, TRAIT_TYPE_FLY, false);
      Cube_SetTraitEnabled(cube, TRAIT_TYPE_ROTATE, true);
      Cube_SetTraitEnabled(cube, TRAIT_TYPE_MOVE, true);
    } else {
      Cube_SetTraitEnabled(cube, TRAIT_TYPE_FLY, true);
      Cube_SetTraitEnabled(cube, TRAIT_TYPE_ROTATE, false);
      Cube_SetTraitEnabled(cube, TRAIT_TYPE_MOVE, false);
    }
    break;
  }

  return true;
}

static const GBA_TileMapRef portal = {
  .width = 3, .height = 6,
  .tiles = GBA_Tiles_Of(
    { .tileId = 168, .vFlip = 0, .hFlip = 0 },
    { .tileId = 169, .vFlip = 0, .hFlip = 0 },
    { .tileId = 170, .vFlip = 0, .hFlip = 0 },
    { .tileId = 176, .vFlip = 0, .hFlip = 0 },
    { .tileId = 177, .vFlip = 0, .hFlip = 0 },
    { .tileId = 178, .vFlip = 0, .hFlip = 0 },
    { .tileId = 184, .vFlip = 0, .hFlip = 0 },
    { .tileId = 185, .vFlip = 0, .hFlip = 0 },
    { .tileId = 186, .vFlip = 0, .hFlip = 0 },
    { .tileId = 171, .vFlip = 0, .hFlip = 0 },
    { .tileId = 172, .vFlip = 0, .hFlip = 0 },
    { .tileId = 173, .vFlip = 0, .hFlip = 0 },
    { .tileId = 179, .vFlip = 0, .hFlip = 0 },
    { .tileId = 180, .vFlip = 0, .hFlip = 0 },
    { .tileId = 181, .vFlip = 0, .hFlip = 0 },
    { .tileId = 187, .vFlip = 0, .hFlip = 0 },
    { .tileId = 188, .vFlip = 0, .hFlip = 0 },
    { .tileId = 189, .vFlip = 0, .hFlip = 0 },
  )
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
