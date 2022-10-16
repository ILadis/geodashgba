
#include <game/course.h>

Course*
Course_GetInstance() {
  static Course course = {0};
  return &course;
}

void
Course_Reset(Course *course) {
  course->redraw = true;
  course->count = 0;
  course->floor = 16;
  course->offset = Vector_Of(0, 0);
  course->grid.bounds = Bounds_Of(0, 0, 1200, 1200);
}

Object*
Course_AddObject(Course *course) {
  int index = course->count++;
  return &course->objects[index];
}

static inline void
Course_FillGrid(Course *course) {
  Cell *grid = &course->grid;
  for (int i = 0; i < course->count; i++) {
    Object *object = &course->objects[i];
    Bounds *viewbox = &object->viewbox;

    Unit unit = Unit_Of(viewbox, object);

    Cell_AddUnit(grid, &unit);
  }
}

static inline Hit
Course_CheckFloorHit(
    Course *course,
    Bounds *hitbox)
{
  Hit hit = {0};

  int dy = (course->floor + 1)*8 - (hitbox->center.y + hitbox->size.y + 1);
  if (dy < 0) {
    hit.delta.y = dy;
  }

  return hit;
}

Hit
Course_CheckHits(
    Course *course,
    Bounds *hitbox)
{
  Iterator iterator;
  Cell_GetUnits(&course->grid, hitbox, &iterator);

  Hit hit = {0};

  while (Iterator_HasNext(&iterator)) {
    Unit *unit = Iterator_GetNext(&iterator);
    Object *object = unit->object;

    Bounds *bounds = &object->hitbox;

    Hit h = Bounds_Intersects(bounds, hitbox);
    if (Hit_IsHit(&h)) hit = Hit_Combine(&hit, &h);
  }

  Hit h = Course_CheckFloorHit(course, hitbox);
  if (Hit_IsHit(&h)) hit = Hit_Combine(&hit, &h);

  return hit;
}

static inline void
Course_Redraw(
    Course *course,
    Camera *camera)
{
  extern const GBA_TileMapRef backgroundTileMap;

  GBA_EnableBackgroundLayer(0, (GBA_BackgroundControl) {
    .size = 1,
    .colorMode = 1,
    .tileSetIndex = 0,
    .tileMapIndex = 8,
    .priority = 3,
  });

  GBA_EnableBackgroundLayer(1, (GBA_BackgroundControl) {
    .size = 0,
    .colorMode = 1,
    .tileSetIndex = 0,
    .tileMapIndex = 14,
    .priority = 2,
  });

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 0);
  GBA_TileMapRef_Blit(&target, 0, 0, &backgroundTileMap);

  Iterator iterator;
  Bounds *viewport = Camera_GetViewport(camera);
  Cell_GetUnits(&course->grid, viewport, &iterator);

  while (Iterator_HasNext(&iterator)) {
    Unit *unit = Iterator_GetNext(&iterator);
    Object *object = unit->object;

    Camera_Draw(camera, &object->position, object->tiles);
  }
}

static inline void
Course_DrawOffset(
    Course *course,
    Camera *camera)
{
  Vector *position = Camera_GetPosition(camera);

  course->offset.x = position->x;
  course->offset.y = position->y;

  GBA_OffsetBackgroundLayer(0, course->offset.x >> 1, course->offset.y >> 1);
  GBA_OffsetBackgroundLayer(1, course->offset.x, course->offset.y);
}

static inline const GBA_Tile*
Course_FloorTile(
    Course *course,
    int y)
{
  static const GBA_Tile empty = {0};
  static const GBA_Tile floor[] = {{ .tileId = 24 }, { .tileId = 10 }};

  int dy = y - course->floor;

  if (dy < 0) {
    return &empty;
  } else if (dy == 0) {
    return &floor[0];
  } else if (dy < 3) {
    return &floor[1];
  } else {
    return &empty;
  }
}

static inline void
Course_DrawFloor(
    Course *course,
    Camera *camera)
{
  // TODO clearing should be done in camera
  Vector *position = Camera_GetPosition(camera);
  Vector *delta = Camera_GetDelta(camera);

  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 1);

  if (delta->x != 0) {
    int x = (position->x >> 3) + (delta->x > 0 ? 30 : 0); // right/left most edge (not visible yet)
    int y = (position->y >> 3);

    for (int row = 0; row < 32; row++, y++) {
      const GBA_Tile *tile = Course_FloorTile(course, y);
      GBA_TileMapRef_BlitTile(&target, x, y, tile);
    }
  }

  if (delta->y != 0) {
    int y = (position->y >> 3) + (delta->y > 0 ? 20 : 0); // top/bottom most edge (not visible yet)
    int x = (position->x >> 3);

    const GBA_Tile *tile = Course_FloorTile(course, y);
    for (int col = 0; col < 32; col++, x++) {
      GBA_TileMapRef_BlitTile(&target, x, y, tile);
    }
  }
}

static inline void
Course_DrawObjects(
    Course *course,
    Camera *camera)
{
  Course_DrawFloor(course, camera);

  Iterator iterator;
  Bounds *viewport = Camera_GetViewport(camera);
  Cell_GetUnits(&course->grid, viewport, &iterator);

  while (Iterator_HasNext(&iterator)) {
    Unit *unit = Iterator_GetNext(&iterator);
    Object *object = unit->object;

    Camera_DrawDelta(camera, &object->position, object->tiles);
  }
}

void
Course_Draw(
    Course *course,
    Camera *camera)
{
  Course_DrawOffset(course, camera);

  if (course->redraw) {
    // FIXME currently a workaround to fill grid
    Course_FillGrid(course);
    Course_Redraw(course, camera);
    course->redraw = false;
  } else {
    Course_DrawObjects(course, camera);
  }
}