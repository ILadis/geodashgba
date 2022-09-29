
#include <game/camera.h>

Camera*
Camera_GetInstance() {
  static Camera camera = {0};
  return &camera;
}

void
Camera_Reset(Camera *camera) {
  camera->position = Vector_Of(0, 0);
  camera->delta = Vector_Of(0, 0);
  camera->viewport = Bounds_Of(120, 80, 120, 80);
  camera->frame = Bounds_Of(54, 96, 22, 40);
  camera->target = NULL;
}

static inline void
Camera_MoveBy(
    Camera *camera,
    Vector *delta)
{
  camera->position.x += delta->x;
  camera->position.y += delta->y;

  camera->viewport.center.x += delta->x;
  camera->viewport.center.y += delta->y;

  camera->frame.center.x += delta->x;
  camera->frame.center.y += delta->y;
}

static inline Vector
Camera_GetTargetDelta(Camera *camera) {
  Vector *target = camera->target;
  Bounds *frame = &camera->frame;

  int dx = target->x - frame->center.x;
  int dy = target->y - frame->center.y;

  Vector delta = Vector_Of(0, 0);

  int px = Math_abs(dx) - frame->size.x;
  int py = Math_abs(dy) - frame->size.y;

  if (px > 0) {
    int sx = Math_signum(dx);
    delta.x = sx * px;
  }

  if (py > 0) {
    int sy = Math_signum(dy);
    delta.y = sy * py;
  }

  return delta;
}

static inline void
Camera_SetDelta(
    Camera *camera,
    Vector *delta)
{
  int dx = ((camera->position.x & 0b0111) + delta->x) >> 3;
  int dy = ((camera->position.y & 0b0111) + delta->y) >> 3;

  camera->delta.x = dx;
  camera->delta.y = dy;
}

void
Camera_Update(Camera *camera) {
  Vector *target = camera->target;

  if (target != NULL) {
    Vector delta = Camera_GetTargetDelta(camera);

    Camera_SetDelta(camera, &delta);
    Camera_MoveBy(camera, &delta);
  }
}

static inline void
Camera_DrawTiles(
    Camera *camera,
    const Vector *position,
    const GBA_TileMapRef *tileMap,
    const Vector from, const Vector to)
{
  GBA_TileMapRef target;
  GBA_TileMapRef_FromBackgroundLayer(&target, 1);

  for (int y = from.y; y < to.y; y++) {
    for (int x = from.x; x < to.x; x++) {
      int i = (y - position->y) * tileMap->width + (x - position->x);

      GBA_Tile *tile = &tileMap->tiles[i];
      GBA_TileMapRef_BlitTile(&target, x, y, tile);
    }
  }
}

void
Camera_DrawDelta(
    Camera *camera,
    const Vector *position,
    const GBA_TileMapRef *tileMap)
{
  int height = tileMap->height;
  int width = tileMap->width;

  int cx = camera->position.x >> 3;
  int cy = camera->position.y >> 3;

  int oy = position->y;
  int ox = position->x;

  int dx = camera->delta.x;
  if (dx != 0) {
    int tx = cx + (dx > 0 ? 30 : 0); // right/left most edge (not visible yet)

    int sx = Math_max(ox, tx);
    int ex = Math_min(ox + width, tx + 1);

    int sy = Math_max(oy, cy);
    int ey = Math_min(oy + height, cy + 21);

    Camera_DrawTiles(camera, position, tileMap, Vector_Of(sx, sy), Vector_Of(ex, ey));
  }

  int dy = camera->delta.y;
  if (dy != 0) {
    int ty = cy + (dy > 0 ? 20 : 0); // top/bottom most edge (not visible yet)

    int sx = Math_max(ox, cx);
    int ex = Math_min(ox + width, cx + 31);

    int sy = Math_max(oy, ty);
    int ey = Math_min(oy + height, ty + 1);

    Camera_DrawTiles(camera, position, tileMap, Vector_Of(sx, sy), Vector_Of(ex, ey));
  }
}

void
Camera_Draw(
    Camera *camera,
    const Vector *position,
    const GBA_TileMapRef *tileMap)
{
  int width = tileMap->width;
  int height = tileMap->height;

  int cx = camera->position.x >> 3;
  int cy = camera->position.y >> 3;

  int oy = position->y;
  int ox = position->x;

  int sx = Math_max(ox, cx);
  int ex = Math_min(ox + width, cx + 31);

  int sy = Math_max(oy, cy);
  int ey = Math_min(oy + height, cy + 21);

  Camera_DrawTiles(camera, position, tileMap, Vector_Of(sx, sy), Vector_Of(ex, ey));
}
