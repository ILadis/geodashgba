#ifndef TEXT_H
#define TEXT_H

#include <gba.h>

typedef struct Glyph {
  const int width;
  const unsigned char *fill;
  const unsigned char *outline;
  const unsigned char *highlight;
} Glyph;

typedef struct Font {
  const int height;
  const Glyph *glyphs[95];
} Font;

typedef struct Printer {
  GBA_TileMapRef *tileMap;
  const Font *font;
  struct { int x, y; } cursor;
  struct {
    int fill;
    int outline;
    int highlight;
    int background;
  } colors;
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
}

static inline void
Printer_SetFillColor(
    Printer *printer,
    int color)
{
  printer->colors.fill = color;
}

static inline void
Printer_SetOutlineColor(
    Printer *printer,
    int color)
{
  printer->colors.outline = color;
}

static inline void
Printer_SetHighlightColor(
    Printer *printer,
    int color)
{
  printer->colors.highlight = color;
}

static inline void
Printer_SetBackgroundColor(
    Printer *printer,
    int color)
{
  printer->colors.background = color;
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
    const char *text);

void
Printer_WriteLine(
    Printer *printer,
    const char *line);

#endif
