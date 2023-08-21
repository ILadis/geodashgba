
#include <text.h>

static inline const Glyph*
Text_GetGlyph(
    Text *text,
    char letter)
{
  const Font *font = text->font;
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
Text_GetWidth(
    Text *text,
    const char *line)
{
  int width = 0;

  do {
    char letter = *(line++);
    const Glyph *glyph = Text_GetGlyph(text, letter);

    if (glyph == NULL) {
      break;
    }

    width += glyph->width + 1;
  } while (true);

  return width;
}

static inline void
Text_SetPixel(
    Text *text,
    int px, int py,
    int color)
{
  GBA_TileMapRef *canvas = text->canvas;
  if (canvas != NULL) {
    GBA_TileMapRef_SetPixel(canvas, px, py, color);
  } else {
    GBA_Bitmap_FillPixel(px, py, GBA_Color_From(color));
  }
}

static void
Text_PrintGlyph(
    Text *text,
    const Glyph *glyph)
{
  const Font *font = text->font;

  int px = text->cursor.x;
  int py = text->cursor.y;

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
        Text_SetPixel(text, px + x, py + y, text->colors.fill);
      }

      else if (outline & 1) {
        Text_SetPixel(text, px + x, py + y, text->colors.outline);
      }

      else if (highlight & 1) {
        Text_SetPixel(text, px + x, py + y, text->colors.highlight);
      }

      else {
        Text_SetPixel(text, px + x, py + y, text->colors.background);
      }

      fill >>= 1;
      outline >>= 1;
      highlight >>= 1;
    }
  }

  text->cursor.x += glyph->width + 1;
}

void
Text_WriteLine(
    Text *text,
    const char *line)
{
  do {
    char letter = *(line++);
    const Glyph *glyph = Text_GetGlyph(text, letter);

    if (glyph == NULL) {
      break;
    }

    Text_PrintGlyph(text, glyph);
  } while (true);

  // TODO increase cursor y on new line character
}
