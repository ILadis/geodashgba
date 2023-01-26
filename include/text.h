#ifndef TEXT_H
#define TEXT_H

#include <gba.h>

typedef struct Glyph {
  int width;
  unsigned char *data;
} Glyph;

typedef struct Font {
  int height;
  const Glyph *glyphs[128];
} Font;

typedef struct Printer {
  GBA_TileMapRef tileMap;
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
  printer->tileMap = *tileMap;
}

static inline void
Printer_SetCursor(
    Printer *printer,
    int x, int y)
{
  printer->cursor.x = x;
  printer->cursor.y = y;
}

// TODO consider foreground / background color to avoid Printer_ClearChar
void
Printer_PutChar(
    Printer *printer,
    int letter,
    int color);

void
Printer_ClearChar(
    Printer *printer,
    int letter,
    int color);

#endif
