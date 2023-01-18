
#include <game/cube.h>
#include <game/particle.h>

Cube*
Cube_GetInstance() {
  static Cube cube = {0};
  return &cube;
}

void
Cube_Reset(Cube *cube) {
  const Cube empty = {0};

  GBA_Sprite *sprite = cube->sprite;
  if (sprite != NULL) {
    GBA_Sprite_Release(sprite);
  }

  *cube = empty;
}

void
Cube_SetPosition(
    Cube *cube,
    const Vector *position)
{
  Body *body = &cube->body;

  static const Dynamics dynamics = {
    .friction = { 60, 0 },
    .gravity  = { 0, 90 },
    .limits   = { 600, 3000 },
  };

  // body is using 8w fixed-point integer
  Body_SetDynamics(body, &dynamics);
  Body_SetPosition(body, position->x << 8, position->y << 8);

  cube->shape = Shape_Of(cube->vertices);
  cube->hitbox = Bounds_Of(position->x, position->y, 8, 8);
  cube->state.current = STATE_UNKNOWN;
}

void
Cube_Update(
    Cube *cube,
    Course *course)
{
  extern void Cube_ApplyMovement(Cube *cube);
  extern void Cube_ApplySpawn(Cube *cube, Course *course);
  extern void Cube_ApplyHit(Cube *cube, Course *course);
  extern void Cube_ApplyRotation(Cube *cube, Course *course);

  Cube_ApplySpawn(cube, course);
  Cube_ApplyMovement(cube);
  Cube_ApplyHit(cube, course);
  Cube_ApplyRotation(cube, course);


  if (Cube_EnteredState(cube, STATE_VICTORY)) {
    Body *body = &cube->body;

    static const Dynamics dynamics = {
      .friction = { 100, 100 },
      .gravity  = { 0, 0 },
      .limits   = { 600, 3000 },
    };

    Body_SetDynamics(body, &dynamics);
    Cube_Accelerate(cube, DIRECTION_RIGHT, 0);
  }

  else if (Cube_InState(cube, STATE_VICTORY) && !Cube_IsMoving(cube)) {
    Body *body = &cube->body;

    static const Dynamics dynamics = {
      .friction = { 0, 0 },
      .gravity  = { 200, 45 },
      .limits   = { 800, 800 },
    };

    Body_SetDynamics(body, &dynamics);
    Body_SetVelocity(body, -150, -800);
  }

  else if (Cube_InState(cube, STATE_GROUNDED) && Cube_IsMoving(cube)) {
    static const Dynamics dynamics = {
      .friction = { 0, 0 },
      .gravity  = { 0, 50 },
      .limits   = { 800, 800 },
    };

    static int timer = 0;
    Vector position = cube->hitbox.center;
    position.x -= 6;
    position.y += 6;

    if (timer++ % 8 == 0) {
      Particle *particle = Particle_NewInstance(&position, &dynamics, 16, 0);
      Particle_SetVelocity(particle, 0, -500);
      Particle_SetSize(particle, PARTICLE_SIZE_SMALL);
    }
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
