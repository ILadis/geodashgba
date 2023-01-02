
#include <game/object.h>

bool
Object_IsHit(
    Object *object,
    Shape *shape)
{
  extern bool Object_HitSpike(Object *object, Shape *shape);
  extern bool Object_HitPad(Object *object, Shape *shape);

  const bool (*hit[TYPE_COUNT])(Object *object, Shape *shape) = {
    [TYPE_SPIKE] = Object_HitSpike,
    [TYPE_PAD] = Object_HitPad,
  };

  if (hit[object->type] != NULL) {
    return hit[object->type](object, shape);
  }

  return true;
}

void
Object_Move(
    Object *object,
    Vector *position)
{
  extern void Object_MoveSpike(Object *object, Vector *position);

  const void (*move[TYPE_COUNT])(Object *object, Vector *position) = {
    [TYPE_SPIKE] = Object_MoveSpike,
  };

  object->hitbox.center.x  += position->x * 8;
  object->hitbox.center.y  += position->y * 8;
  object->viewbox.center.x += position->x * 8;
  object->viewbox.center.y += position->y * 8;

  if (move[object->type] != NULL) {
    move[object->type](object, position);
  }
}

void
Object_Draw(
    Object *object,
    GBA_TileMapRef *target)
{
  extern void Object_DrawDisk(Object *object, GBA_TileMapRef *target);
  extern void Object_DrawBox(Object *object, GBA_TileMapRef *target);
  extern void Object_DrawBoxWithPole(Object *object, GBA_TileMapRef *target);
  extern void Object_DrawSpike(Object *object, GBA_TileMapRef *target);
  extern void Object_DrawPad(Object *object, GBA_TileMapRef *target);

  const void (*draw[TYPE_COUNT])(Object *object, GBA_TileMapRef *target) = {
    [TYPE_DISK] = Object_DrawDisk,
    [TYPE_BOX] = Object_DrawBox,
    [TYPE_BOX_WITH_POLE] = Object_DrawBoxWithPole,
    [TYPE_SPIKE] = Object_DrawSpike,
    [TYPE_PAD] = Object_DrawPad,
  };

  if (draw[object->type] != NULL) {
    draw[object->type](object, target);
  }
}
