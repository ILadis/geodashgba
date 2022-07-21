
import { TileMap } from 'utils.mjs';
import * as Writers from 'writers.mjs';

function TileMapExporter() {
  this.name = 'GBA Tilemap C-Source file';
  this.extension = 'c';
}

TileMapExporter.prototype.write = function(map, filePath) {
  let file = new TextFile(filePath, TextFile.WriteOnly);;
  let writer = new Writers.TileMapWriter(file);

  let layers = TileMap.fromMap(map);
  for (let [name, tiles] of layers) {
    writer.write(name, tiles);
  }

  file.commit();
};

var exporter = new TileMapExporter();
tiled.registerMapFormat(exporter.name, exporter);
