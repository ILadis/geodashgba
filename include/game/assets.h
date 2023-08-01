#ifndef ASSETS_H
#define ASSETS_H

#include <gba.h>
#include <text.h>

extern const unsigned int spritesTiles[512];
extern const unsigned short spritesPal[256];
extern const unsigned int tilesTiles[4096];
extern const unsigned short tilesPal[256];

extern const signed short bezlut[256];

extern const Font consoleFont;
extern const Font hudFont;

extern const GBA_TileMapRef courseBackgroundTileMap;
extern const GBA_TileMapRef selectTopOverlayTileMap;
extern const GBA_TileMapRef selectLeftEdgeOverlayTileMap;
extern const GBA_TileMapRef selectRightEdgeOverlayTileMap;
extern const GBA_TileMapRef selectLevelBoxTileMap;
extern const GBA_TileMapRef progressPlayTileMap;
extern const GBA_TileMapRef progressSelectorTileMap;

#endif
