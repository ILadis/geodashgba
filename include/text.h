#ifndef TEXT_H
#define TEXT_H

#include <gba.h>

typedef struct Glyph {
  const int width;
  const unsigned char *outline;
  const unsigned char *highlight;
  const unsigned char *background;
} Glyph;

typedef struct Font {
  const int height;
  const Glyph *glyphs[64];
} Font;

typedef struct Printer {
  GBA_TileMapRef *tileMap;
  const Font *font;
  struct { int x, y; } cursor;
} Printer;

static inline void
Printer_SetFont(
    Printer *printer,
    const Font *font)
{
  printer->font = font;
}

static inline void
Printer_SetCanvas(
    Printer *printer,
    GBA_TileMapRef *tileMap)
{
  printer->tileMap = tileMap;
  // TODO prepare canvas (set tile ids and apply background color)
}

static inline void
Printer_SetCursor(
    Printer *printer,
    int x, int y)
{
  printer->cursor.x = x;
  printer->cursor.y = y;
}

int
Printer_GetTextWidth(
    Printer *printer,
    char *text);

void
Printer_WriteLine(
    Printer *printer,
    char *line,
    int color);

#endif
