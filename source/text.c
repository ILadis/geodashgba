
#include <text.h>

void
Printer_Init(Printer *printer, const Font *font, GBA_TileMapRef *tileMap) {
  printer->font = font;
  printer->tileMap = *tileMap;
}

void
Printer_SetCursor(Printer *printer, int x, int y) {
  printer->cursor.x = x;
  printer->cursor.y = y;
}

void
Printer_PutChar(
    Printer *printer,
    int letter,
    int color)
{
  const Font *font = printer->font;
  const Glyph *glyph = font->glyphs[letter];
  if (glyph == NULL) return;

  int cx = printer->cursor.x;
  int cy = printer->cursor.y;

  for (int y = 0; y < font->height; y++) {
    // TODO support glyphs wider than 8 pixels (char is only 8 bits)
    unsigned char row = glyph->data[y];

    for (int x = 0; x < glyph->width; x++, row >>= 1) {
      if (row & 1) {
        GBA_TileMapRef_SetPixel(&printer->tileMap, cx + x, cy + y, color);
      }
    }
  }

  printer->cursor.x += glyph->width;
}

void
Printer_ClearChar(
    Printer *printer,
    int letter,
    int color)
{
  const Font *font = printer->font;
  const Glyph *glyph = font->glyphs[letter];
  if (glyph == NULL) return;

  int cx = printer->cursor.x;
  int cy = printer->cursor.y;

  for (int y = 0; y < font->height; y++) {
    for (int x = 0; x < glyph->width; x++) {
      GBA_TileMapRef_SetPixel(&printer->tileMap, cx + x, cy + y, color);
    }
  }

  printer->cursor.x += glyph->width;
}
