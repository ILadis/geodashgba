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

void
Printer_Init(
    Printer *printer,
    const Font *font,
    GBA_TileMapRef *tileMap);

void
Printer_SetCursor(
    Printer *printer,
    int x, int y);

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
