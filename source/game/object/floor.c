
#include <game/object.h>

bool
Object_CreateFloor(Object *object) {
  const Object floor = (Object) {
    .solid = true,
    .deadly = false,
    .type = OBJECT_TYPE_FLOOR,
  };

  Object_AssignFrom(object, &floor);

  return true;
}
