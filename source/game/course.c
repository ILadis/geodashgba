
#include <game/course.h>

Course*
Course_GetInstance() {
  static Course course = {0};
  return &course;
}

void
Course_Reset(Course *course) {
  course->redraw = true;
  course->floor = (Bounds) {
    .center = Vector_Of(120, 148),
    .size = Vector_Of(120, 13),
  };
  course->boxes[0] = (Bounds) {
    .center = Vector_Of(120, 128),
    .size = Vector_Of(8, 9),
  };
  course->boxes[1] = (Bounds) {
    .center = Vector_Of(168, 120),
    .size = Vector_Of(8, 17),
  };
}

Hit
Course_CheckHits(
    Course *course,
    Cube *cube)
{
  Bounds *hitbox = Cube_GetHitbox(cube);

  Hit hit = Bounds_Intersects(&course->floor, hitbox);
  if (Hit_IsHit(&hit)) return hit;

  for (int i = 0; i < 2; i++) {
    hit = Bounds_Intersects(&course->boxes[i], hitbox);
    if (Hit_IsHit(&hit)) return hit;
  }

  return hit;
}

void
Course_Draw(
    Course *course,
    Camera *camera)
{
  static GBA_TileMapRef target;
  extern const GBA_TileMapRef backgroundTileMap, course1TileMap;

  if (course->redraw) {
    GBA_EnableBackgroundLayer(0, (GBA_BackgroundControl) {
      .size = 0, // should be 1 (blitting does not work, must be drawn in 32x32 chucks)
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

    GBA_TileMapRef_FromBackgroundLayer(&target, 0);
    GBA_TileMapRef_Blit(&target, 0, 0, &backgroundTileMap);

    GBA_TileMapRef_FromBackgroundLayer(&target, 1);
    GBA_TileMapRef_Blit(&target, 0, 0, &course1TileMap);

    course->redraw = false;
  }

  Vector *position = Camera_GetPosition(camera);

  course->scroll.x = position->x;
  course->scroll.y = position->y;

  GBA_OffsetBackgroundLayer(0, course->scroll.x, course->scroll.y);
  GBA_OffsetBackgroundLayer(1, course->scroll.x, course->scroll.y);
}