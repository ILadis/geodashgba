
export const Assets = {
  iterate:  function*(asset, type) {
    let count = asset[type + 'Count'];
    for (let i = 0; i < count; i++) {
      let item = asset[type + 'At'](i);
      yield item;
    }
  }
};

export function TileMap(layer) {
  this.layer = layer;
}

TileMap.fromMap = function(map) {
  let tiles = new Map();

  for (let layer of Assets.iterate(map, 'layer')) {
    if (!layer.isTileLayer) continue;
    tiles.set(layer.name, new TileMap(layer));
  }

  return tiles;
};

TileMap.prototype.width = function() {
  return this.layer.width;
};

TileMap.prototype.height = function() {
  return this.layer.height;
};

TileMap.prototype.at = function(x, y) {
  let tile = this.layer.tileAt(x, y);
  let cell = this.layer.cellAt(x, y);

  if (!tile) return null;

  let id    = tile.id;
  let vFlip = cell.flippedVertically || cell.flippedAntiDiagonally ? 1 : 0;
  let hFlip = cell.flippedHorizontally || cell.flippedAntiDiagonally ? 1 : 0;

  return { id, vFlip, hFlip };
};
