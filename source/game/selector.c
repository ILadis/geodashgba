
#include <game/selector.h>

Selector*
Selector_GetInstance() {
  static Selector selector = { .redraw = true };
  return &selector;
}

void
Selector_Update(Selector *selector) {

}

void
Selector_Draw(Selector *selector) {
  extern const GBA_TileMapRef selectTopOverlayTileMap;
  extern const GBA_TileMapRef selectLeftEdgeOverlayTileMap;
  extern const GBA_TileMapRef selectRightEdgeOverlayTileMap;

  static const GBA_BackgroundControl layer = {
    .size = 0, // 256x256
    .colorMode = 1,
    .tileSetIndex = 0,
    .tileMapIndex = 20,
    .priority = 1,
  };

  if (selector->redraw) {
    GBA_EnableBackgroundLayer(2, layer);

    GBA_TileMapRef target;
    GBA_TileMapRef_FromBackgroundLayer(&target, 2);

    GBA_TileMapRef_Blit(&target,  5,  0, &selectTopOverlayTileMap);
    GBA_TileMapRef_Blit(&target,  0, 16, &selectLeftEdgeOverlayTileMap);
    GBA_TileMapRef_Blit(&target, 25, 16, &selectRightEdgeOverlayTileMap);
  }

  selector->redraw = false;
}
