
#include <text.h>

static void
Printer_PutChar(
    Printer *printer,
    int letter,
    int color)
{
  const Font *font = printer->font;
  const Glyph *glyph = font->glyphs[letter];

  static const Glyph fallback = {0};

  if (glyph == NULL) {
    glyph = &fallback;
  }

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

void
Printer_WriteLine(
    Printer *printer,
    char *line,
    int color)
{
  const Font *font = printer->font;
  const int limit = length(font->glyphs);

  do {
    char letter = *(line++);
    if (letter == '\0') {
      break;
    }

    if (letter >= 'a' && letter <= 'z') {
      letter = 'A' + (letter - 'a');
    }

    int index = letter - 'A';
    if (index < 0 || index > limit) {
      continue;
    }

    Printer_PutChar(printer, index, color);
  } while (true);

  // TODO increase cursor y
}
