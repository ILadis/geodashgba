
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

  int index = letter - ' ';
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
    const char *text)
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

static inline void
Printer_SetPixel(
    Printer *printer,
    int px, int py,
    int color)
{
  GBA_TileMapRef *tileMap = printer->tileMap;
  if (tileMap != NULL) {
    GBA_TileMapRef_SetPixel(tileMap, px, py, color);
  } else {
    GBA_Bitmap_FillPixel(px, py, GBA_Color_From(color));
  }
}

static void
Printer_PrintGlyph(
    Printer *printer,
    const Glyph *glyph)
{
  const Font *font = printer->font;

  int px = printer->cursor.x;
  int py = printer->cursor.y;

  int index = 0;
  for (int y = 0; y < font->height; y++) {
    unsigned char fill = 0, outline = 0, highlight = 0;

    for (int x = 0; x < glyph->width; x++) {
      if (x % 8 == 0) {
        fill = glyph->fill[index];
        outline = glyph->outline[index];
        highlight = glyph->highlight[index++];
      }

      if (fill & 1) {
        Printer_SetPixel(printer, px + x, py + y, printer->colors.fill);
      }

      else if (outline & 1) {
        Printer_SetPixel(printer, px + x, py + y, printer->colors.outline);
      }

      else if (highlight & 1) {
        Printer_SetPixel(printer, px + x, py + y, printer->colors.highlight);
      }

      else {
        Printer_SetPixel(printer, px + x, py + y, printer->colors.background);
      }

      fill >>= 1;
      outline >>= 1;
      highlight >>= 1;
    }
  }

  printer->cursor.x += glyph->width + 1;
}

void
Printer_WriteLine(
    Printer *printer,
    const char *text)
{
  do {
    char letter = *(text++);
    const Glyph *glyph = Printer_GetGlyph(printer, letter);

    if (glyph == NULL) {
      break;
    }

    Printer_PrintGlyph(printer, glyph);
  } while (true);

  // TODO increase cursor y on new line character
}
