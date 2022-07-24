
#include <game/cube.h>

Cube*
Cube_GetInstance() {
  static Cube cube = {0};
  return &cube;
}

void
Cube_Reset(Cube *cube) {
  cube->velocity = Vector_Of(0, 0);
  cube->acceleration = Vector_Of(0, 0);
  cube->gravity = Vector_Of(0, 90);
  cube->friction = Vector_Of(60, 0);
  cube->position = Vector_Of(50 << 8, 20 << 8);
  cube->hitbox = Bounds_Of(50, 20, 8, 8);
  cube->state = STATE_UNKNOWN;
}

void
Cube_Jump(Cube *cube) {
  // maybe course should decide whether jumping/accelerating is allowed or not? (if we have different modes/sections)
  if (cube->state == STATE_GROUNDED) {
    cube->velocity.y = -1500;
    cube->acceleration.y = 0;
    cube->state = STATE_JUMPING;
  }
}

void
Cube_Accelerate(
    Cube *cube,
    Direction direction)
{
  if (Direction_IsHorizontal(direction)) {
    int ax = (direction == DIRECTION_LEFT) ? -160 : 160;
    cube->acceleration.x = ax;
  }
}

void
Cube_Update(Cube *cube) {
  int x = cube->velocity.x + cube->acceleration.x + cube->gravity.x;
  int y = cube->velocity.y + cube->acceleration.y + cube->gravity.y;

  // apply friction
  if (x < 0) {
    x += Math_min(-x, cube->friction.x);
  } else if (x > 0) {
    x -= Math_min(+x, cube->friction.x);
  }

  // set new positions
  cube->position.x += x;
  cube->position.y += y;
  cube->hitbox.center.x = (cube->position.x >> 8) + 8;
  cube->hitbox.center.y = (cube->position.y >> 8) + 8;

  cube->acceleration.x = 0; // better set somwhere else

  // max speeds
  x = Math_clamp(x, -600, +600);
  y = Math_clamp(y, -1300, +1300);

  // set new velocity
  cube->velocity.x = x;
  cube->velocity.y = y;
}

void
Cube_TakeHit(
    Cube *cube,
    Hit *hit)
{
  if (hit->delta.y < 0) {
    cube->velocity.y = 0;
    cube->acceleration.y = 0;
    cube->hitbox.center.y += hit->delta.y;
    cube->position.y = (cube->hitbox.center.y - 8) << 8;
    cube->state = STATE_GROUNDED;
  }

  else if (hit->delta.x != 0) {
    cube->velocity.x = 0;
    cube->acceleration.x = 0;
    cube->hitbox.center.x += hit->delta.x;
    cube->position.x = (cube->hitbox.center.x - 8) << 8;
  }
}

static inline GBA_Sprite*
Cube_LoadSprite(Cube *cube) {
  static const GBA_Sprite base = {
    .colorMode = 0,
    .paletteBank = 0,
    .shape = 0,
    .size = 1,
    .tileId = 0,
    .gfxMode = 0,
    .objMode = 0,
    .priority = 0,
  };

  GBA_Sprite *sprite = cube->sprite;

  if (sprite == NULL) {
    cube->sprite = sprite = GBA_Sprite_Allocate();

    sprite->attr0 = base.attr0;
    sprite->attr1 = base.attr1;
    sprite->attr2 = base.attr2;

    GBA_Affine *affine = GBA_Affine_Allocate();
    GBA_Sprite_SetAffine(sprite, affine);

    GBA_Sprite_SetRotation(sprite, 64);
  }

  return sprite;
}

void
Cube_Draw(
    Cube *cube,
    Camera *camera)
{
  GBA_Sprite *sprite = Cube_LoadSprite(cube);

  Vector position = {
    .x = cube->hitbox.center.x - 8,
    .y = cube->hitbox.center.y - 8,
  };

  Camera_RelativeTo(camera, &position);
  GBA_Sprite_SetPosition(sprite, position.x, position.y);
}
