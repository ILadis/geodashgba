
#include <game/particle.h>

static Prefab *prefabs = Prefab_Create(Particle, 32);

static bool
Particle_Reset(Particle *particle);

void
Particle_ResetAll() {
  Prefab_ForEach(prefabs, (Action) Particle_Reset);
  Prefab_ResetAll(prefabs);
}

static bool
Particle_Update(Particle *particle);

void
Particle_UpdateAll() {
  Prefab_ForEach(prefabs, (Action) Particle_Update);
}

static bool
Particle_Draw(Particle *particle);

void
Particle_DrawAll() {
  Prefab_ForEach(prefabs, (Action) Particle_Draw);
}

Particle*
Particle_NewInstance(
    const Vector *position,
    int life, int delay)
{
  Particle *particle = Prefab_NewInstance(prefabs);

  Body *body = &particle->body;

  static const Dynamics dynamics = (Dynamics) {
    .friction = 0,
    .gravity  = { 0, 0 },
    .limits   = { 5000, 5000 },
  };

  // body is using 8w fixed-point integer
  Body_SetDynamics(body, &dynamics);
  Body_SetPosition(body, (position->x << 8) + 4, (position->y << 8) + 4);

  int angle = Math_rand();
  int velocity = 1500;

  int dx = (Math_cos(angle) * velocity) >> 8;
  int dy = (Math_sin(angle) * velocity) >> 8;

  Body_SetVelocity(body, dx, dy);

  particle->life = life;
  particle->delay = delay;

  return particle;
}

static bool
Particle_Reset(Particle *particle) {
  const Particle empty = {0};

  GBA_Sprite *sprite = particle->sprite;
  if (sprite != NULL) {
    GBA_Sprite_Release(sprite);
  }

  *particle = empty;

  return false;
}

static bool
Particle_Update(Particle *particle) {
  int delay = particle->delay -= 1;
  if (delay > 0) {
    return true;
  }

  int life = particle->life -= 1;
  if (life <= 0) {
    return Particle_Reset(particle);
  }

  Body *body = &particle->body;
  Body_Update(body);

  return true;
}

static inline GBA_Sprite*
Particle_LoadSprite(Particle *particle) {
  static GBA_Sprite base = {
    .colorMode = 0,
    .paletteBank = 0,
    .shape = 0,
    .size = 0,
    .tileId = 4,
    .gfxMode = 0,
    .objMode = 0,
    .priority = 0,
  };

  GBA_Sprite *sprite = particle->sprite;

  if (sprite == NULL) {
    particle->sprite = sprite = GBA_Sprite_Allocate();

    // TODO find better way to specify/choose tile id
    static int tileId = 4;
    base.tileId = tileId++;
    if (tileId > 6) tileId = 4;

    sprite->attr0 = base.attr0;
    sprite->attr1 = base.attr1;
    sprite->attr2 = base.attr2;
  }

  return sprite;
}

static bool
Particle_Draw(Particle *particle) {
  int delay = particle->delay;
  if (delay > 0) {
    return true;
  }

  GBA_Sprite *sprite = Particle_LoadSprite(particle);

  Body *body = &particle->body;
  Vector *position = Body_GetPosition(body);

  // revert 8w fixed-point integer
  int x = position->x >> 8;
  int y = position->y >> 8;

  Bounds bounds = {
    .size = Vector_Of(4, 4),
    .center = Vector_Of(x, y),
  };

  Camera *camera = Camera_GetInstance();
  if (Camera_InViewport(camera, &bounds)) {
    Camera_RelativeTo(camera, &bounds.center);
    GBA_Sprite_SetPosition(sprite, bounds.center.x, bounds.center.y);
    GBA_Sprite_SetObjMode(sprite, 0); // show sprite
  } else {
    GBA_Sprite_SetObjMode(sprite, 2); // hide sprite
  }

  return true;
}
