
#include <game/cube.h>

Cube*
Cube_GetInstance() {
  static Cube cube = {0};
  return &cube;
}

void
Cube_Reset(
    Cube *cube,
    Vector *spawn)
{
  Movement *movement = &cube->movement;

  // movement is using 8w fixed-point integer
  Movement_SetVelocityLimit(movement, 600, 1300);
  Movement_SetGravity(movement, 90);
  Movement_SetFriction(movement, 60);
  Movement_SetPosition(movement, (spawn->x << 8) + 8, (spawn->y << 8) + 8);

  cube->hitbox = Bounds_Of(spawn->x, spawn->y, 8, 8);
  cube->state.current = STATE_UNKNOWN;
  cube->state.previous = STATE_UNKNOWN;
}

void
Cube_Jump(Cube *cube) {
  if (Cube_InState(cube, STATE_GROUNDED)) {
    cube->movement.velocity.current.y = -1500;
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

static inline void
Cube_ApplyMovement(Cube *cube) {
  Bounds *hitbox = &cube->hitbox;
  Movement *movement = &cube->movement;

  Movement_Update(movement);
  Vector *position = Movement_GetPosition(movement);

  // revert 8w fixed-point integer
  hitbox->center.x = position->x >> 8;
  hitbox->center.y = position->y >> 8;
}

static inline void
Cube_ApplyHit(
    Cube *cube,
    Course *course)
{
  Bounds *hitbox = &cube->hitbox;
  Hit hit = Course_CheckHits(course, hitbox);

  Movement *movement = &cube->movement;
  State state = STATE_AIRBORNE;

  if (hit.delta.y < 0) {
    hitbox->center.y += hit.delta.y + 1; // stay in contact with ground
    movement->velocity.current.y = 0;
    movement->position.y = cube->hitbox.center.y << 8;
    state = STATE_GROUNDED;
  }

  if (hit.delta.y > 0) {
    hitbox->center.y += hit.delta.y;
    movement->velocity.current.y = 0;
    movement->position.y = cube->hitbox.center.y << 8;
  }

  if (hit.delta.x != 0) {
    hitbox->center.x += hit.delta.x;
    movement->velocity.current.x = 0;
    movement->position.x = cube->hitbox.center.x << 8;
  }

  Cube_SetState(cube, state);
}

static inline int
Cube_CalculateRotationVelocity(
    Cube *cube,
    Course *course)
{
  Cube shadow = (Cube) {
    .movement = cube->movement,
    .hitbox = cube->hitbox,
    .state = cube->state,
  };

  int sign = Math_signum(shadow.movement.velocity.current.x);
  if (sign == 0) {
    return 0;
  }

  // do not slow down vertical movement
  Movement_SetFriction(&shadow.movement, 0);

  int frames = 0;
  do {
    frames++;

    Cube_ApplyMovement(&shadow);
    Cube_ApplyHit(&shadow, course);
  } while (!Cube_EnteredState(&shadow, STATE_GROUNDED));

  // do a 1/4 rotation
  int alpha = 64;
  // do a 2/4 rotation
  if (frames >= 24) alpha = 128;
  // do a 3/4 rotation
  if (frames >= 40) alpha = 192;
  // do a 4/4 rotation
  if (frames >= 64) alpha = 256;

  int velocity = sign * Math_div(alpha, frames);

  return velocity;
}

static void
Cube_ApplyRotation(
    Cube *cube,
    Course *course)
{
  if (Cube_EnteredState(cube, STATE_GROUNDED)) {
    cube->rotation.velocity = 0;
    cube->rotation.angle = 64;
  }

  if (Cube_EnteredState(cube, STATE_AIRBORNE)) {
    int velocity = Cube_CalculateRotationVelocity(cube, course);
    cube->rotation.velocity = velocity;
    cube->rotation.angle = 64;
  }
}

void
Cube_Update(
    Cube *cube,
    Course *course)
{
  Cube_ApplyMovement(cube);
  Cube_ApplyHit(cube, course);
  Cube_ApplyRotation(cube, course);
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

  int rotation = cube->rotation.angle -= cube->rotation.velocity;

  Vector position = {
    .x = cube->hitbox.center.x - 16,
    .y = cube->hitbox.center.y - 17,
  };

  Camera_RelativeTo(camera, &position);
  GBA_Sprite_SetPosition(sprite, position.x, position.y);
  GBA_Sprite_SetRotation(sprite, rotation);
}
