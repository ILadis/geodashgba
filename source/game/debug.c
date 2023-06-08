
#include <gba.h>
#include <log.h>
#include <text.h>

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

static Printer*
Debug_GetPrinter() {
  static bool once = true;
  static Printer printer = {0};

  if (once) {
    GBA_EnableMode(3);
    GBA_Bitmap_FillRect(0, 0, 240, 160, (GBA_Color) {0});

    extern const Font consoleFont;
    Printer_SetFont(&printer, &consoleFont);
    Printer_SetFillColor(&printer, 0xF21238);
    Printer_SetBackgroundColor(&printer, 0x000000);

    Printer_SetCanvas(&printer, NULL);
    Printer_SetCursor(&printer, 0, 0);

    once = false;
  }

  return &printer;
}

static void
Debug_Print(const char *message) {
  Printer *printer = Debug_GetPrinter();

  if (message[0] == '\n') {
    int y = printer->cursor.y + printer->font->height;
    Printer_SetCursor(printer, 0, y);
  }
  else {
    Printer_WriteLine(printer, message);
  }
}

Logger*
Debug_GetLogger() {
  static Logger logger = { Debug_Print };
  return &logger;
}
