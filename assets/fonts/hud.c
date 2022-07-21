#include <text.h>

static const Glyph glyph0 = (Glyph) {
  .width = 5,
  .data = (unsigned char[]) {
    0b00001111,
    0b00001001,
    0b00001001,
    0b00001001,
    0b00001111,
  }
};

static const Glyph glyph1 = (Glyph) {
  .width = 5,
  .data = (unsigned char[]) {
    0b00000100,
    0b00000100,
    0b00000100,
    0b00000100,
    0b00000100,
  }
};

static const Glyph glyph2 = (Glyph) {
  .width = 5,
  .data = (unsigned char[]) {
    0b00001111,
    0b00001000,
    0b00001111,
    0b00000001,
    0b00001111,
  }
};

static const Glyph glyph3 = (Glyph) {
  .width = 5,
  .data = (unsigned char[]) {
    0b00001111,
    0b00001000,
    0b00001111,
    0b00001000,
    0b00001111,
  }
};

static const Glyph glyph4 = (Glyph) {
  .width = 5,
  .data = (unsigned char[]) {
    0b00001001,
    0b00001001,
    0b00001111,
    0b00001000,
    0b00001000,
  }
};

static const Glyph glyph5 = (Glyph) {
  .width = 5,
  .data = (unsigned char[]) {
    0b00001111,
    0b00000001,
    0b00001111,
    0b00001000,
    0b00001111,
  }
};

static const Glyph glyph6 = (Glyph) {
  .width = 5,
  .data = (unsigned char[]) {
    0b00001111,
    0b00000001,
    0b00001111,
    0b00001001,
    0b00001111,
  }
};

static const Glyph glyph7 = (Glyph) {
  .width = 5,
  .data = (unsigned char[]) {
    0b00001111,
    0b00001000,
    0b00001000,
    0b00001000,
    0b00001000,
  }
};

static const Glyph glyph8 = (Glyph) {
  .width = 5,
  .data = (unsigned char[]) {
    0b00001111,
    0b00001001,
    0b00001111,
    0b00001001,
    0b00001111,
  }
};

static const Glyph glyph9 = (Glyph) {
  .width = 5,
  .data = (unsigned char[]) {
    0b00001111,
    0b00001001,
    0b00001111,
    0b00001000,
    0b00001111,
  }
};

const Font hudFont = (Font) {
  .height = 5,
  .glyphs = {
    ['0'] = &glyph0,
    ['1'] = &glyph1,
    ['2'] = &glyph2,
    ['3'] = &glyph3,
    ['4'] = &glyph4,
    ['5'] = &glyph5,
    ['6'] = &glyph6,
    ['7'] = &glyph7,
    ['8'] = &glyph8,
    ['9'] = &glyph9,
  },
};
