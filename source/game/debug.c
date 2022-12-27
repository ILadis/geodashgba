
#include <gba.h>

#include <game/camera.h>
#include <game/cube.h>
#include <game/course.h>

static inline void
Debug_DrawHitbox(
    Camera *camera,
    Bounds *hitbox,
    GBA_Color color)
{
  Vector center = hitbox->center;
  Vector size = hitbox->size;

  if (!Camera_InViewport(camera, hitbox)) {
    return;
  }

  Camera_RelativeTo(camera, &center);

  int px1 = center.x - size.x;
  int py1 = center.y - size.y;

  int px2 = center.x + size.x;
  int py2 = center.y + size.y;

  GBA_Bitmap_DrawRect(px1, py1, px2, py2, color);
}

void
Debug_DrawHitboxes() {
  const GBA_Color black = { .value = 0  };
  const GBA_Color green = { .green = 31 };
  const GBA_Color blue  = { .blue = 31  };

  GBA_EnableMode(3);
  GBA_Bitmap_FillRect(0, 0, 240, 160, black);

  Camera *camera = Camera_GetInstance();

  Cube *cube = Cube_GetInstance();
  Bounds *hitbox = &cube->hitbox;
  Debug_DrawHitbox(camera, hitbox, blue);

  Course *course = Course_GetInstance();

  Iterator iterator;
  Bounds *viewport = Camera_GetViewport(camera);

  Chunk *current = Course_GetChunkAt(course, course->index);
  Grid_GetUnits(&current->grid, viewport, &iterator);

  while (Iterator_HasNext(&iterator)) {
    Unit *unit = Iterator_GetNext(&iterator);
    Object *object = unit->object;

    Bounds *hitbox = &object->hitbox;
    Debug_DrawHitbox(camera, hitbox, green);
  }

  Chunk *next = Course_GetChunkAt(course, course->index + 1);
  Grid_GetUnits(&next->grid, viewport, &iterator);

  while (Iterator_HasNext(&iterator)) {
    Unit *unit = Iterator_GetNext(&iterator);
    Object *object = unit->object;

    Bounds *hitbox = &object->hitbox;
    Debug_DrawHitbox(camera, hitbox, green);
  }
}
