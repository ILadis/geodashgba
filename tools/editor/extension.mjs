
import * as Utils from 'utils.mjs';
import * as Writers from 'writers.mjs';

const TileMapExporter = {
  name: 'GBA Tilemap C-Source file',
  extension: 'c',
};

TileMapExporter.write = function(map, filePath) {
  let file = new TextFile(filePath, TextFile.WriteOnly);
  let writer = new Writers.TileMapWriter(file);

  let tileMaps = Utils.tileMapFromMap(map);
  for (let tileMap of tileMaps) {
    writer.write(tileMap);
  }

  file.commit();
};

tiled.registerMapFormat(TileMapExporter.name, TileMapExporter);

const ColliderTool = {
  name: 'Find Tile Colliders',
};

ColliderTool.activated = function() {
  let layer = new ObjectGroup('colliders');
  this.map.addLayer(layer);

  let tileMaps = Utils.tileMapFromMap(this.map);
  for (let tileMap of tileMaps) {
    let colliders = Utils.tileCollidersFormTileMap(tileMap);

    let index = 1;
    for (let collider of colliders) {
      let obj = new MapObject('Collider #' + index);
      obj.shape = MapObject.Rectangle;

      obj.x = collider.position.x;
      obj.y = collider.position.y;
      obj.width = collider.width;
      obj.height = collider.height;

      layer.addObject(obj);
      index++;
    }
  }
};

tiled.registerTool(ColliderTool.name, ColliderTool);
