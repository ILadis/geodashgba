
#include <text.h>

void
Printer_PutChar(
    Printer *printer,
    int letter,
    int color)
{
  const Font *font = printer->font;
  const Glyph *glyph = font->glyphs[letter - 'A'];
  if (glyph == NULL) return;

  int px = printer->cursor.x;
  int py = printer->cursor.y;

  int index = 0;

  for (int y = 0; y < font->height; y++) {
    unsigned char outline = 0, highlight = 0, background = 0;

    for (int x = 0; x < glyph->width; x++) {
      if (x % 8 == 0) {
        outline = glyph->outline[index];
        highlight = glyph->highlight[index];
        background = glyph->background[index++];
      }

      if (outline & 1) {
        GBA_TileMapRef_SetPixel(&printer->tileMap, px + x, py + y, 16);
      }

      if (highlight & 1) {
        GBA_TileMapRef_SetPixel(&printer->tileMap, px + x, py + y, color + 1);
      }

      if (background & 1) {
        GBA_TileMapRef_SetPixel(&printer->tileMap, px + x, py + y, 22);
      }

      outline >>= 1;
      highlight >>= 1;
      background >>= 1;
    }
  }

  printer->cursor.x += glyph->width + 1;
}
