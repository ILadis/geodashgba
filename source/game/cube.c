
#include <game/cube.h>

Cube*
Cube_GetInstance() {
  static Cube cube = {0};

  static SpawnTrait spawn = {0};
  static MoveTrait move = {0};
  static RotateTrait rotate = {0};
  static HitTrait hit = {0};

  if (cube.traits[0] == NULL) {
    SpawnTrait_BindTo(&spawn, &cube);
    MoveTrait_BindTo(&move, &cube);
    RotateTrait_BindTo(&rotate, &cube);
    HitTrait_BindTo(&hit, &cube);
  }

  return &cube;
}

void
Cube_Reset(Cube *cube) {
  GBA_Sprite *sprite = cube->sprite;
  if (sprite != NULL) {
    GBA_Sprite_Release(sprite);
  }

  cube->state.current = STATE_UNKNOWN;
  cube->state.previous = STATE_UNKNOWN;

  cube->success = false;
  cube->sprite = NULL;
}

static inline GBA_Sprite*
Cube_LoadSprite(Cube *cube) {
  static const GBA_Sprite base = {
    .colorMode = 0,
    .paletteBank = 0,
    .shape = GBA_SPRITE_SHAPE_OF(16, 16),
    .size = GBA_SPRITE_SIZE_OF(16, 16),
    .tileId = 0,
    .objMode = GBA_SPRITE_MODE_RENDER,
    .priority = 0,
  };

  GBA_Sprite *sprite = cube->sprite;

  if (sprite == NULL) {
    cube->sprite = sprite = GBA_Sprite_Allocate();
    sprite->attr0 = base.attr0;
    sprite->attr1 = base.attr1;
    sprite->attr2 = base.attr2;

    GBA_Affine *affine = GBA_Affine_Allocate();
    GBA_Sprite_SetAffine(sprite, affine, true);
    GBA_Sprite_SetRotation(sprite, 0);
  }

  return sprite;
}

void
Cube_Draw(
    Cube *cube,
    Camera *camera)
{
  GBA_Sprite *sprite = Cube_LoadSprite(cube);

  if (Cube_InState(cube, STATE_DESTROYED)) {
    GBA_Sprite_SetObjMode(sprite, GBA_SPRITE_MODE_HIDE);
  }
  else {
    const Body *body = Cube_GetBody(cube);
    int rotation = Cube_GetRotation(cube);
    int gravity = Math_signum(body->dynamics->gravity.y);

    const Vector *center = Cube_GetPosition(cube);

    Vector position = {
      .x = center->x - 16,
      .y = center->y - (16 + gravity), // draw 1px higher because of contact with ground
    };

    Camera_RelativeTo(camera, &position);
    GBA_Sprite_SetPosition(sprite, position.x, position.y);
    GBA_Sprite_SetRotation(sprite, rotation);
    GBA_Sprite_SetObjMode(sprite, GBA_SPRITE_MODE_AFFINE2);
  }
}
