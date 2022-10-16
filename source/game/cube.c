
#include <game/cube.h>

Cube*
Cube_GetInstance() {
  static Cube cube = {0};
  return &cube;
}

void
Cube_Reset(Cube *cube) {
  Movement *movement = &cube->movement;

  // initial position
  const int x = 50;
  const int y = 20;

  // movement is using 8w fixed-point integer
  Movement_SetVelocityLimit(movement, 600, 1300);
  Movement_SetGravity(movement, 90);
  Movement_SetFriction(movement, 60);
  Movement_SetPosition(movement, x << 8, y << 8);

  cube->hitbox = Bounds_Of(x, y, 8, 8);
  cube->state = STATE_UNKNOWN;
}

void
Cube_Jump(Cube *cube) {
  // maybe course should decide whether jumping/accelerating is allowed or not? (if we have different modes/sections)
  if (cube->state == STATE_GROUNDED) {
    cube->movement.velocity.current.y = -1500;
    cube->movement.acceleration.y = 0;
    cube->state = STATE_JUMPING;
  }
}

void
Cube_Accelerate(
    Cube *cube,
    Direction direction,
    int speed)
{
  const Vector *vector = Vector_FromDirection(direction);

  int ax = speed * vector->x;
  int ay = speed * vector->y;

  Movement *movement = &cube->movement;
  Movement_SetAcceleration(movement, ax, ay);
}

void
Cube_Update(Cube *cube) {
  Movement *movement = &cube->movement;
  Bounds *hitbox = &cube->hitbox;

  Movement_Update(movement);
  Vector *position = Movement_GetPosition(movement);

  // revert 8w fixed-point integer
  hitbox->center.x = (position->x >> 8) + 8;
  hitbox->center.y = (position->y >> 8) + 8;
}

void
Cube_TakeHit(
    Cube *cube,
    Hit *hit)
{
  Movement *movement = &cube->movement;
  Bounds *hitbox = &cube->hitbox;

  if (hit->delta.y != 0) {
    movement->velocity.current.y = 0;
    movement->acceleration.y = 0;
    hitbox->center.y += hit->delta.y;
    movement->position.y = (cube->hitbox.center.y - 8) << 8;
    cube->state = hit->delta.y < 0 ? STATE_GROUNDED : cube->state;
  }

  else if (hit->delta.x != 0) {
    movement->velocity.current.x = 0;
    movement->acceleration.x = 0;
    hitbox->center.x += hit->delta.x;
    movement->position.x = (cube->hitbox.center.x - 8) << 8;
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
