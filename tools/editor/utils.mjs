
export const Assets = {
  iterate:  function*(asset, type) {
    let count = asset[type + 'Count'];
    for (let i = 0; i < count; i++) {
      let item = asset[type + 'At'](i);
      yield item;
    }
  }
};

export function TileMap(name, x, y, width, height, tiles) {
  this.name = name;
  this.x = x;
  this.y = y;
  this.width = width;
  this.height = height;
  this.at = tiles;
}

TileMap.fromMap = function*(map) {
  for (let layer of Assets.iterate(map, 'layer')) {
    if (!layer.isTileLayer) continue;
    let { name, width, height } = layer;
    yield new TileMap(name, 0, 0, width, height, tiles.bind(layer));
  }

  function tiles(x, y) {
    let tile = this.tileAt(x, y);
    let cell = this.cellAt(x, y);

    if (!tile) return null;

    let id    = tile.id;
    let vFlip = cell.flippedVertically || cell.flippedAntiDiagonally ? 1 : 0;
    let hFlip = cell.flippedHorizontally || cell.flippedAntiDiagonally ? 1 : 0;

    return { id, vFlip, hFlip };
  };
};

TileMap.fromArray = function(array) {
  let width = array[0].length;
  let height = array.length;

  let tiles = (x, y) => {
    let id = array[y] ? array[y][x] : null;
    return id ? { id, vFlip: 0, hFlip: 0 } : null;
  };

  return new TileMap('', 0, 0, width, height, tiles);
};

TileMap.prototype.contains = function(x, y) {
  return x >= this.x && x < this.x + this.width
    && y >= this.y && y < this.y + this.height;
};

TileMap.prototype.slice = function(x, y, width, height) {
  let tiles = (dx, dy) => this.at(x + dx, y + dy);
  return new TileMap(this.name, x, y, width, height, tiles);
};

export function TileMapSlicer(tileMap) {
  this.tileMap = tileMap;
}

TileMapSlicer.prototype.sliceAt = function(x, y) {
  let width = 0, height = 0;
  while (this.tileMap.at(x + width, y)) {
    width++;
  }

  if (width == 0) {
    return null;
  }

  do {
    var length = 0;

    height++;
    while (this.tileMap.at(x + length, y + height)) {
      length++;
    }
  } while (length == width);

  return this.tileMap.slice(x, y, width, height);
};

TileMapSlicer.prototype.sliceAll = function() {
  let slices = new Array();

  let width = this.tileMap.width;
  let height = this.tileMap.height;

  for (let y = 0; y < height; y++) {
    next: for (let x = 0; x < width; x++) {

      for (let slice of slices) {
        if (slice.contains(x, y)) {
          continue next;
        }
      }

      let slice = this.sliceAt(x, y);
      if (slice) {
        slices.push(slice);
      }
    }
  }

  return slices;
};
