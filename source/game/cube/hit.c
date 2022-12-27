
#include <game/cube.h>

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

void
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
