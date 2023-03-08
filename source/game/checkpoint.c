
#include <game/checkpoint.h>

Checkpoint*
Checkpoint_GetInstance() {
  static Checkpoint checkpoint = {0};
  return &checkpoint;
}

void
Checkpoint_Reset(Checkpoint *checkpoint) {
  const Checkpoint empty = {0};

  for (int i = 0; i < length(checkpoint->sprites); i++) {
    GBA_Sprite *sprite = checkpoint->sprites[i];
    if (sprite != NULL) {
      GBA_Sprite_Release(sprite);
    }
  }

  *checkpoint = empty;
}

const Vector*
Checkpoint_GetLastPosition(Checkpoint *checkpoint) {
  return checkpoint->last;
}

bool
Checkpoint_RemoveLastPosition(Checkpoint *checkpoint) {
  int count = checkpoint->count - 1;
  if (count < 0) {
    return false;
  }

  int index = checkpoint->index - 1 % length(checkpoint->positions);
  Vector *last = checkpoint->last;

  last->x = 0;
  last->y = 0;

  checkpoint->last = count == 0 ? NULL : &checkpoint->positions[index];
  checkpoint->index = index;
  checkpoint->count = count;

  return true;
}

void
Checkpoint_AddPosition(
    Checkpoint *checkpoint,
    const Vector *position)
{
  int index = (checkpoint->index + 1) % length(checkpoint->positions);
  int count = Math_min(checkpoint->count + 1, length(checkpoint->positions));

  Vector *last = &checkpoint->positions[index];

  last->x = position->x;
  last->y = position->y;

  checkpoint->last = last;
  checkpoint->index = index;
  checkpoint->count = count;
}

static inline void
Checkpoint_ReleaseSprite(
    Checkpoint *checkpoint,
    int index)
{
  GBA_Sprite *sprite = checkpoint->sprites[index];
  if (sprite != NULL) {
    GBA_Sprite_Release(sprite);
    checkpoint->sprites[index] = NULL;
  }
}

static inline GBA_Sprite*
Checkpoint_LoadSprite(
    Checkpoint *checkpoint,
    int index)
{
  static const GBA_Sprite base = {
    .colorMode = 0,
    .paletteBank = 0,
    .shape = 2,
    .size = 0,
    .tileId = 16,
    .gfxMode = 0,
    .objMode = 0,
    .priority = 0,
  };

  GBA_Sprite *sprite = checkpoint->sprites[index];

  if (sprite == NULL) {
    sprite = GBA_Sprite_Allocate();
    sprite->attr0 = base.attr0;
    sprite->attr1 = base.attr1;
    sprite->attr2 = base.attr2;

    checkpoint->sprites[index] = sprite;
  }

  return sprite;
}

void
Checkpoint_Draw(
    Checkpoint *checkpoint,
    Camera *camera)
{
  Bounds bounds = Bounds_Of(0, 0, 4, 8);

  for (int i = 0; i < length(checkpoint->positions); i++) {
    const Vector *position = &checkpoint->positions[i];
    const Vector zero = Vector_Of(0, 0);

    if (Vector_Equals(position, &zero)) {
      Checkpoint_ReleaseSprite(checkpoint, i);
      continue;
    }

    bounds.center.x = position->x;
    bounds.center.y = position->y;

    if (Camera_InViewport(camera, &bounds)) {
      Camera_RelativeTo(camera, &bounds.center);

      GBA_Sprite *sprite = Checkpoint_LoadSprite(checkpoint, i);

      Vector lower = Bounds_Lower(&bounds);
      GBA_Sprite_SetPosition(sprite, lower.x, lower.y);
    }
    else {
      Checkpoint_ReleaseSprite(checkpoint, i);
    }
  }
}
