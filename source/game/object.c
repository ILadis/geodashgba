
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

  bool (*const hit[OBJECT_TYPE_COUNT])(Object *object, Shape *shape) = {
    [OBJECT_TYPE_SPIKE] = Object_HitSpike,
    [OBJECT_TYPE_PAD] = Object_HitPad,
    [OBJECT_TYPE_RING] = Object_HitRing,
    [OBJECT_TYPE_COIN] = Object_HitCoin,
    [OBJECT_TYPE_PORTAL] = Object_HitPortal,
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

  void (*const move[OBJECT_TYPE_COUNT])(Object *object, Vector *position) = {
    [OBJECT_TYPE_SPIKE] = Object_MoveSpike,
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

  void (*const draw[OBJECT_TYPE_COUNT])(Object *object, GBA_TileMapRef *target) = {
    [OBJECT_TYPE_DISK] = Object_DrawDisk,
    [OBJECT_TYPE_BOX] = Object_DrawBox,
    [OBJECT_TYPE_BOX_WITH_POLE] = Object_DrawBoxWithPole,
    [OBJECT_TYPE_BOX_WITH_CHAINS] = Object_DrawBoxWithChains,
    [OBJECT_TYPE_PIT] = Object_DrawPit,
    [OBJECT_TYPE_SPIKE] = Object_DrawSpike,
    [OBJECT_TYPE_PAD] = Object_DrawPad,
    [OBJECT_TYPE_RING] = Object_DrawRing,
    [OBJECT_TYPE_COIN] = Object_DrawCoin,
    [OBJECT_TYPE_PORTAL] = Object_DrawPortal,
    [OBJECT_TYPE_GOAL_WALL] = Object_DrawGoalWall,
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

  bool (*const animate[OBJECT_TYPE_COUNT])(Object *object, GBA_TileMapRef *target, int frame) = {
    [OBJECT_TYPE_BOX_WITH_POLE] = Object_AnimateBoxWithPole,
    [OBJECT_TYPE_PAD] = Object_AnimatePad,
    [OBJECT_TYPE_COIN] = Object_AnimateCoin,
  };

  if (animate[object->type] != NULL) {
    return animate[object->type](object, target, frame);
  }

  return false;
}
