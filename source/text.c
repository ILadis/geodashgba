
#include <text.h>

static inline const Glyph*
Printer_GetGlyph(
    Printer *printer,
    char letter)
{
  const Font *font = printer->font;
  static const Glyph fallback = {0};
  const int limit = length(font->glyphs);

  if (letter == '\0') {
    return NULL;
  }

  if (letter >= 'a' && letter <= 'z') {
    letter = 'A' + (letter - 'a');
  }

  int index = letter - 'A';
  if (index < 0 || index > limit) {
    return &fallback;
  }

  const Glyph *glyph = font->glyphs[index];
  if (glyph == NULL) {
    return &fallback;
  }

  return glyph;
}

int
Printer_GetTextWidth(
    Printer *printer,
    char *text)
{
  int width = 0;

  do {
    char letter = *(text++);
    const Glyph *glyph = Printer_GetGlyph(printer, letter);

    if (glyph == NULL) {
      break;
    }

    width += glyph->width + 1;
  } while (true);

  return width;
}

static void
Printer_PrintGlyph(
    Printer *printer,
    const Glyph *glyph,
    int color)
{
  const Font *font = printer->font;

  int px = printer->cursor.x;
  int py = printer->cursor.y;

  GBA_TileMapRef *tileMap = printer->tileMap;
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
        GBA_TileMapRef_SetPixel(tileMap, px + x, py + y, 16);
      }

      if (highlight & 1) {
        GBA_TileMapRef_SetPixel(tileMap, px + x, py + y, color + 1);
      }

      if (background & 1) {
        GBA_TileMapRef_SetPixel(tileMap, px + x, py + y, 22);
      }

      outline >>= 1;
      highlight >>= 1;
      background >>= 1;
    }
  }

  printer->cursor.x += glyph->width + 1;
}

void
Printer_WriteLine(
    Printer *printer,
    char *text,
    int color)
{
  do {
    char letter = *(text++);
    const Glyph *glyph = Printer_GetGlyph(printer, letter);

    if (glyph == NULL) {
      break;
    }

    Printer_PrintGlyph(printer, glyph, color);
  } while (true);

  // TODO increase cursor y
}
