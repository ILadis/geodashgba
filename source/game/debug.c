
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

int
Debug_GetElapsedTime() {
  static const GBA_TimerData overflows[] = { {-16}, {-1024}, {0} };
  static bool once = true;

  if (once) {
    GBA_StartTimerCascade(GBA_TIMER_FREQUENCY_1024, overflows);
    once = false;
  }

  int seconds = GBA_GetTimerValue(2, overflows);
  int millis = GBA_GetTimerValue(1, overflows);

  return seconds * 1024 + millis;
}

void
Debug_LogElapsedTime(
    char *message,
    int timestamp)
{
  Logger *logger = Logger_GetInstance();
  int timespan = Debug_GetElapsedTime() - timestamp;

  if (timespan > 5) {
    Logger_Print(logger, message);
    Logger_Print(logger, "  0x");
    Logger_PrintHex16(logger, timespan);
    Logger_PrintNewline(logger);
  }
}

static Text*
Debug_GetText() {
  static bool once = true;
  static Text text = {0};

  if (once) {
    GBA_EnableMode(3);
    GBA_Bitmap_FillRect(0, 0, 240, 160, (GBA_Color) {0});

    extern const Font consoleFont;
    Text_SetFont(&text, &consoleFont);
    Text_SetFillColor(&text, 0xF21238);
    Text_SetBackgroundColor(&text, 0x000000);

    Text_SetCanvas(&text, NULL);
    Text_SetCursor(&text, 0, 0);

    once = false;
  }

  return &text;
}

static void
Debug_Print(const char *message) {
  Text *text = Debug_GetText();

  if (message[0] == '\n') {
    Text_SetCursorNewline(text);
  } else {
    Text_WriteLine(text, message);
  }
}

Logger*
Debug_GetLogger() {
  static Logger logger = Logger_CreateNew(Debug_Print);
  return &logger;
}
