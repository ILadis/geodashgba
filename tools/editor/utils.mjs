
export const Assets = {
  iterate:  function*(asset, type) {
    let count = asset[type + 'Count'];
    for (let i = 0; i < count; i++) {
      let item = asset[type + 'At'](i);
      yield item;
    }
  }
};

export const Vector = {
  zero: function() {
    return Object.seal({ x: 0, y: 0 });
  },
  from: function(x, y) {
    return Object.seal({ x, y });
  },
  clone: function(vector) {
    return Object.seal({ x: vector.x, y: vector.y });
  }
};

export function Bounds(position, width, height) {
  this.position = position;
  this.width = width;
  this.height = height;
}

Bounds.clone = function(bounds) {
  return new Bounds(Vector.clone(bounds.position), bounds.width, bounds.height);
};

Bounds.prototype.append = function(other) {
  if (matchVertical(this)) {
    this.width += other.width;
    return true;
  }

  if (matchHorizontal(this)) {
    this.height += other.height;
    return true;
  }

  return false;

  function matchVertical(bounds) {
    return bounds.height == other.height
      && bounds.position.x + bounds.width == other.position.x;
  }

  function matchHorizontal(bounds) {
    return bounds.width == other.width
      && bounds.position.y + bounds.height == other.position.y;
  }
};

Bounds.prototype.intersects = function(other) {
  return this.position.x < other.position.x + other.width &&
    this.position.x + this.width > other.position.x &&
    this.position.y < other.position.y + other.height &&
    this.height + this.position.y > other.position.y;
};

export function TileMap(name, position, width, height, tiles) {
  this.name = name;
  this.position = position;
  this.width = width;
  this.height = height;
  this.at = tiles;
}

TileMap.fromMap = function*(map) {
  const { tileWidth, tileHeight } = map;

  for (let layer of Assets.iterate(map, 'layer')) {
    if (!layer.isTileLayer) continue;
    let { name, width, height } = layer;
    yield new TileMap(name, Vector.zero(), width, height, tiles.bind(layer));
  }

  function tiles(x, y) {
    let tile = this.tileAt(x, y);
    let cell = this.cellAt(x, y);

    if (!tile) return null;

    let object = !!tile.objectGroup ? tile.objectGroup.objects[0] : null;
    if (object) {
      let position = Vector.from(x * tileWidth + object.x, y * tileHeight + object.y);
      var shape = new Bounds(position, object.width, object.height);
    }

    let id = tile.id;
    let vFlip = cell.flippedVertically || cell.flippedAntiDiagonally ? 1 : 0;
    let hFlip = cell.flippedHorizontally || cell.flippedAntiDiagonally ? 1 : 0;

    return { id, vFlip, hFlip, shape };
  }
};

// TODO add shapes and fix tests
TileMap.fromArray = function(array) {
  let width = array[0].length;
  let height = array.length;

  let tiles = (x, y) => {
    let id = array[y] ? array[y][x] : null;
    return id ? { id, vFlip: 0, hFlip: 0 } : null;
  };

  return new TileMap('', Vector.zero(), width, height, tiles);
};

export const tileMapFromMap = TileMap.fromMap;
export const tileMapFromArray = TileMap.fromArray;

TileMap.prototype.slice = function(x, y, width, height) {
  let tiles = (dx, dy) => this.at(x + dx, y + dy);
  return new TileMap(this.name, Vector.from(x, y), width, height, tiles);
};

export function TileColliderFinder(tileMap) {
  this.tileMap = tileMap;
}

TileColliderFinder.prototype.tileVisited = function(tile) {
  if (!tile || !tile.shape) {
    return true;
  }

  for (let collider of this.colliders) {
    if (collider.intersects(tile.shape)) {
      return true;
    }
  }

  return false;
};

TileColliderFinder.prototype.processTile = function(x, y) {
  const tileAt = (dx, dy) => this.tileMap.at(x + dx, y + dy);

  var tile = tileAt(0, 0);
  if (this.tileVisited(tile)) {
    return false;
  }

  let collider = Bounds.clone(tile.shape);

  let dx = 0;
  do {
    var tile = tileAt(++dx, 0);
  } while (!this.tileVisited(tile) && collider.append(tile.shape));

  let dy = 0;
  while (true) {
    var tile = tileAt(0, ++dy);
    if (this.tileVisited(tile)) {
      break;
    }

    let bounds = Bounds.clone(tile.shape);

    let dx = 0;
    do {
      var tile = tileAt(++dx, dy);
    } while (!this.tileVisited(tile) && bounds.append(tile.shape));

    if (!collider.append(bounds)) {
      break;
    }
  }

  this.colliders.push(collider);

  return true;
};

TileColliderFinder.prototype.findAll = function() {
  this.colliders = new Array();

  let width = this.tileMap.width;
  let height = this.tileMap.height;

  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++) {
      this.processTile(x, y);
    }
  }

  return this.colliders;
};

export function tileCollidersFormTileMap(tileMap) {
  let finder = new TileColliderFinder(tileMap);
  return finder.findAll();
}
