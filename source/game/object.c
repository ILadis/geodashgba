
#include <game/object.h>

bool
Object_IsHit(
    Object *object,
    Shape *shape)
{
  extern bool Object_HitSpike(Object *object, Shape *shape);
  extern bool Object_HitPad(Object *object, Shape *shape);
  extern bool Object_HitRing(Object *object, Shape *shape);
  extern bool Object_HitCoin(Object *object, Shape *shape);
  extern bool Object_HitPortal(Object *object, Shape *shape);

  bool (*const hit[TYPE_COUNT])(Object *object, Shape *shape) = {
    [TYPE_SPIKE] = Object_HitSpike,
    [TYPE_PAD] = Object_HitPad,
    [TYPE_RING] = Object_HitRing,
    [TYPE_COIN] = Object_HitCoin,
    [TYPE_PORTAL] = Object_HitPortal,
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

  void (*const move[TYPE_COUNT])(Object *object, Vector *position) = {
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
  extern void Object_DrawBoxWithChains(Object *object, GBA_TileMapRef *target);
  extern void Object_DrawPit(Object *object, GBA_TileMapRef *target);
  extern void Object_DrawSpike(Object *object, GBA_TileMapRef *target);
  extern void Object_DrawPad(Object *object, GBA_TileMapRef *target);
  extern void Object_DrawRing(Object *object, GBA_TileMapRef *target);
  extern void Object_DrawCoin(Object *object, GBA_TileMapRef *target);
  extern void Object_DrawPortal(Object *object, GBA_TileMapRef *target);
  extern void Object_DrawGoalWall(Object *object, GBA_TileMapRef *target);

  void (*const draw[TYPE_COUNT])(Object *object, GBA_TileMapRef *target) = {
    [TYPE_DISK] = Object_DrawDisk,
    [TYPE_BOX] = Object_DrawBox,
    [TYPE_BOX_WITH_POLE] = Object_DrawBoxWithPole,
    [TYPE_BOX_WITH_CHAINS] = Object_DrawBoxWithChains,
    [TYPE_PIT] = Object_DrawPit,
    [TYPE_SPIKE] = Object_DrawSpike,
    [TYPE_PAD] = Object_DrawPad,
    [TYPE_RING] = Object_DrawRing,
    [TYPE_COIN] = Object_DrawCoin,
    [TYPE_PORTAL] = Object_DrawPortal,
    [TYPE_GOAL_WALL] = Object_DrawGoalWall,
  };

  if (draw[object->type] != NULL) {
    draw[object->type](object, target);
  }
}

bool
Object_Animate(
    Object *object,
    GBA_TileMapRef *target,
    int frame)
{
  extern bool Object_AnimateBoxWithPole(Object *object, GBA_TileMapRef *target, int frame);
  extern bool Object_AnimatePad(Object *object, GBA_TileMapRef *target, int frame);
  extern bool Object_AnimateCoin(Object *object, GBA_TileMapRef *target, int frame);

  bool (*const animate[TYPE_COUNT])(Object *object, GBA_TileMapRef *target, int frame) = {
    [TYPE_BOX_WITH_POLE] = Object_AnimateBoxWithPole,
    [TYPE_PAD] = Object_AnimatePad,
    [TYPE_COIN] = Object_AnimateCoin,
  };

  if (animate[object->type] != NULL) {
    return animate[object->type](object, target, frame);
  }

  return false;
}
