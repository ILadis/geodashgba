#if defined NOGBA && !__has_include(<assets/tiles/backgrounds.c>)

#include <gba.h>

const GBA_TileMapRef selectTopOverlayTileMap = {0};
const GBA_TileMapRef selectLeftEdgeOverlayTileMap = {0};
const GBA_TileMapRef selectRightEdgeOverlayTileMap = {0};
const GBA_TileMapRef selectLevelBoxTileMap = {0};
const GBA_TileMapRef courseBackgroundTileMap = {0};

#endif
