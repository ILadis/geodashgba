
export function TileMapWriter(file) {
  this.file = file;
}

TileMapWriter.prototype.write = function(tileMap) {
  let name = tileMap.name;

  this.file.writeLine('#include <gba.h>');
  this.file.writeLine('');
  this.file.writeLine('const GBA_TileMapRef '+ name +'TileMap = (GBA_TileMapRef) {');

  let width = tileMap.width;
  let height = tileMap.height;

  this.file.writeLine('  .width = ' + width + ',');
  this.file.writeLine('  .height = ' + height + ',');
  this.file.writeLine('  .tiles = (GBA_Tile[]) {');

  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++) {
      let tile = tiles.at(x, y);

      this.file.write('    ');
      this.file.writeLine(!tile ? '{ 0 },' : '{'
        + '.tileId = ' + tile.id + ', '
        + '.vFlip = ' + tile.vFlip + ', '
        + '.hFlip = ' + tile.hFlip + ' '
        + '},');
    }
  }

  this.file.writeLine('  },');
  this.file.writeLine('};');

  return this;
};
