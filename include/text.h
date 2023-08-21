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

typedef struct Text {
  GBA_TileMapRef *canvas;
  const Font *font;
  struct { int x, y; } cursor;
  // TODO consider adding bounds
  struct {
    int fill;
    int outline;
    int highlight;
    int background;
  } colors;
} Text;

static inline void
Text_SetFont(Text *text, const Font *font) {
  text->font = font;
}

static inline void
Text_SetCanvas(Text *text, GBA_TileMapRef *canvas) {
  text->canvas = canvas;
}

static inline void
Text_SetFillColor(Text *text, int color) {
  text->colors.fill = color;
}

static inline void
Text_SetOutlineColor(Text *text, int color) {
  text->colors.outline = color;
}

static inline void
Text_SetHighlightColor(Text *text, int color) {
  text->colors.highlight = color;
}

static inline void
Text_SetBackgroundColor(Text *text, int color) {
  text->colors.background = color;
}

static inline void
Text_SetCursor(Text *text, int x, int y) {
  text->cursor.x = x;
  text->cursor.y = y;
}

static inline void
Text_SetCursorNewline(Text *text) {
  int y = text->cursor.y + text->font->height;
  Text_SetCursor(text, 0, y);
}

int
Text_GetWidth(
    Text *text,
    const char *line);

void
Text_WriteLine(
    Text *text,
    const char *line);

#endif
