
#include <game/particle.h>

static Prefab *prefabs = Prefab_Create(Particle, 32);

void
Particle_ResetAll() {
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
Particle_NewInstance(Vector *position) {
  Particle *particle = Prefab_NewInstance(prefabs);

  Movement *movement = &particle->movement;

  // movement is using 8w fixed-point integer
  Movement_SetVelocityLimit(movement, 5000, 5000);
  Movement_SetPosition(movement, (position->x << 8) + 4, (position->y << 8) + 4);

  int angle = Math_rand();
  int velocity = 1500;

  int dx = (Math_cos(angle) * velocity) >> 8; // from 16w to 8w fixed-point integer
  int dy = (Math_sin(angle) * velocity) >> 8;

  Movement_SetVelocity(movement, dx, dy);

  particle->ttl = 16;

  return particle;
}

static bool
Particle_Update(Particle *particle) {
  Movement *movement = &particle->movement;
  Movement_Update(movement);

  int ttl = particle->ttl -= 1;
  if (ttl <= 0) {
    GBA_Sprite *sprite = particle->sprite;
    if (sprite != NULL) {
      GBA_Sprite_Release(sprite);
      particle->sprite = NULL;
    }

    return false;
  }

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
  GBA_Sprite *sprite = Particle_LoadSprite(particle);

  Movement *movement = &particle->movement;
  Vector *position = Movement_GetPosition(movement);

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
