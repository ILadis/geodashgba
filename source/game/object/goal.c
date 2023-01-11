
#include <game/object.h>

bool
Object_CreateGoal(Object *object) {
  Bounds hitbox  = Bounds_Of(12, 12, 12, 12);
  Bounds viewbox = Bounds_Of(12, 12, 12, 12);

  object->hitbox  = hitbox;
  object->viewbox = viewbox;

  object->solid = false;
  object->deadly = false;
  object->type = TYPE_GOAL;

  return true;
}
