
#include <game/cube.h>

Cube*
Cube_GetInstance() {
  static Cube cube = {0};
  return &cube;
}

void
Cube_SetPosition(
    Cube *cube,
    const Vector *position)
{
  Body *body = &cube->body;

  static const Dynamics dynamics = (Dynamics) {
    .friction = 60,
    .gravity  = { 0, 90 },
    .maxvel   = { 600, 1300 },
  };

  // body is using 8w fixed-point integer
  Body_SetDynamics(body, &dynamics);
  Body_SetPosition(body, position->x << 8, position->y << 8);

  cube->hitbox = Bounds_Of(position->x, position->y, 8, 8);
}

void
Cube_Jump(Cube *cube) {
  if (Cube_InState(cube, STATE_GROUNDED)) {
    cube->body.velocity.y = -1500;
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

  Body *body = &cube->body;
  Body_SetAcceleration(body, ax, ay);
}

static inline void
Cube_ApplyMovement(Cube *cube) {
  Bounds *hitbox = &cube->hitbox;
  Body *body = &cube->body;

  Body_Update(body);
  Vector *position = Body_GetPosition(body);

  // revert 8w fixed-point integer
  hitbox->center.x = position->x >> 8;
  hitbox->center.y = position->y >> 8;

  cube->rotation.angle -= cube->rotation.velocity;
}

static inline void
Cube_ResolveHit(Cube *cube, int dy) {
  Bounds *hitbox = &cube->hitbox;
  hitbox->center.y += dy;

  Body *body = &cube->body;
  body->velocity.y = 0;
  body->position.y = cube->hitbox.center.y << 8;
}

static inline void
Cube_HaltMovement(Cube *cube) {
  Body *body = &cube->body;

  const Dynamics *dynamics = Dynamics_OfZero();
  Body_SetDynamics(body, dynamics);
  Body_SetVelocity(body, 0, 0);
  Body_SetAcceleration(body, 0, 0);
}

static inline void
Cube_ApplyHit(
    Cube *cube,
    Course *course)
{
  Bounds *hitbox = &cube->hitbox;
  Hit hit = Course_CheckHits(course, hitbox);

  State state = STATE_AIRBORNE;

  if (hit.delta.x != 0) {
    Cube_HaltMovement(cube);
    state = STATE_DESTROYED;
  }

  else {
    if (hit.delta.y < 0) {
      Cube_ResolveHit(cube, hit.delta.y + 1); // stay in contact with ground
      state = STATE_GROUNDED;
    }
    else if (hit.delta.y > 0) {
      Cube_ResolveHit(cube, hit.delta.y);
    }
  }

  Cube_SetState(cube, state);
}

static inline int
Cube_CalculateRotationVelocity(
    Cube *cube,
    Course *course)
{
  Cube shadow = (Cube) {
    .body = cube->body,
    .hitbox = cube->hitbox,
    .state = cube->state,
  };

  int sign = Math_signum(shadow.body.velocity.x);
  if (sign == 0) {
    return 0;
  }

  int frames = 0;
  do {
    frames++;

    Cube_ApplyMovement(&shadow);
    Cube_ApplyHit(&shadow, course);
  } while (Cube_InState(&shadow, STATE_AIRBORNE));

  // do a 1/4 rotation
  int alpha = 64;
  // do a 4/4 rotation
  if (frames >= 64) alpha = 256;
  // do a 3/4 rotation
  else if (frames >= 40) alpha = 192;
  // do a 2/4 rotation
  else if (frames >= 24) alpha = 128;

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

  else if (Cube_EnteredState(cube, STATE_AIRBORNE)) {
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

  if (Cube_InState(cube, STATE_DESTROYED)) {
    GBA_Sprite_SetObjMode(sprite, 2);
  }
  else {
    int rotation = cube->rotation.angle;

    Vector position = {
      .x = cube->hitbox.center.x - 16,
      .y = cube->hitbox.center.y - 17, // draw 1px higher because of contact with ground
    };

    Camera_RelativeTo(camera, &position);
    GBA_Sprite_SetPosition(sprite, position.x, position.y);
    GBA_Sprite_SetRotation(sprite, rotation);
    GBA_Sprite_SetObjMode(sprite, 3);
  }
}
