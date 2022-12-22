
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
    .limits   = { 600, 1300 },
  };

  // body is using 8w fixed-point integer
  Body_SetDynamics(body, &dynamics);
  Body_SetPosition(body, position->x << 8, position->y << 8);

  cube->shape = Shape_Of(cube->vertices);
  cube->hitbox = Bounds_Of(position->x, position->y, 8, 8);
  cube->state.current = STATE_UNKNOWN;
}

void
Cube_Jump(
    Cube *cube,
    int speed)
{
  if (Cube_InState(cube, STATE_GROUNDED)) {
    const Dynamics *dynamics = cube->body.dynamics;
    int sy = Math_signum(dynamics->gravity.y);
    cube->body.velocity.y = -1 * sy * speed;
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

static void
Cube_HitCallback(
    Unit *unit,
    Object *object,
    Hit *hit)
{
  Cube *cube = unit->object;
  Shape *shape = &cube->shape;

  if (Cube_InState(cube, STATE_DESTROYED)) return;

  // detailed hit check using cube shape
  if (!Object_IsHit(object, shape)) return;

  if (object->deadly) {
    Cube_HaltMovement(cube);
    cube->state.current = STATE_DESTROYED;
  }

  else if (object->solid) {
    // TODO refactor/improve this
    int vx = cube->body.velocity.x >> 8;
    int vy = cube->body.velocity.y >> 8;

    int cx = cube->body.position.x >> 8;
    int cy = cube->body.position.y >> 8;

    Raycast ray = Raycast_Of(cx - vx, cy - vy, vx, vy, 8);

    Bounds hitbox = Bounds_Enlarge(&object->hitbox, &cube->hitbox.size);
    Hit hit2 = Raycast_Intersects(&ray, &hitbox);

    bool dead = hit->delta.x != 0;
    if (Hit_IsHit(&hit2)) {
      dead = hit2.delta.x != 0;
      hit->delta.y = hit2.position.y - cy;
    }

    if (dead) {
      Cube_HaltMovement(cube);
      cube->state.current = STATE_DESTROYED;
    }

    else {
      if (hit->delta.y < 0) {
        Cube_ResolveHit(cube, hit->delta.y + 1); // +1 to stay in contact with ground
        cube->state.current = STATE_GROUNDED;
      }
      else if (hit->delta.y > 0) {
        Cube_ResolveHit(cube, hit->delta.y);
      }
    }
  }
}

static inline void
Cube_ApplyShape(Cube *cube) {
  Bounds *hitbox = &cube->hitbox;
  Vector *vertices = &cube->vertices[0];

  int angle = cube->rotation.angle;

  Vector *position = &hitbox->center;

  int dx = (Math_sin(angle) * hitbox->size.x) >> 8;
  int dy = (Math_cos(angle) * hitbox->size.y) >> 8;

  vertices[0].x = position->x - dx;
  vertices[0].y = position->y - dy;

  vertices[1].x = position->x + dx;
  vertices[1].y = position->y - dy;

  vertices[2].x = position->x + dx;
  vertices[2].y = position->y + dy;

  vertices[3].x = position->x - dx;
  vertices[3].y = position->y + dy;
}

static inline void
Cube_ApplyHit(
    Cube *cube,
    Course *course)
{
  cube->state.previous = cube->state.current;

  if (Cube_InState(cube, STATE_DESTROYED)) {
    cube->state.current = STATE_DESTROYED;
  } else {
    cube->state.current = STATE_AIRBORNE;

    Cube_ApplyShape(cube);

    Unit unit = Unit_Of(&cube->hitbox, cube);
    Course_CheckHits(course, &unit, Cube_HitCallback);
  }
}

static inline int
Cube_CalculateRotationVelocity(
    Cube *cube,
    Course *course)
{
  Cube shadow = (Cube) {
    .body   = cube->body,
    .hitbox = cube->hitbox,
    .state  = cube->state,
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
