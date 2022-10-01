
import './test.mjs';

import { TileMap, TileMapSlicer } from './utils.mjs';

test('TileMap.slice should return expected slice', () => {
  // arrange
  let tileMap = TileMap.fromArray([
    [ 0, 0, 1, 1, 0, 0, 0],
    [ 0, 0, 2, 3, 0, 0, 0],
    [ 0, 0, 0, 4, 1, 0, 0],
  ]);

  // act
  let slice = tileMap.slice(2, 0, 2, 2);

  // assert
  assert(2, slice.x);
  assert(0, slice.y);
  assert(2, slice.width);
  assert(2, slice.height);

  assert(1, slice.at(0, 0).id);
  assert(1, slice.at(1, 0).id);
  assert(2, slice.at(0, 1).id);
  assert(3, slice.at(1, 1).id);
});

test('TileMapSlicer.sliceAll should return expected slices', () => {
  // arrange
  let tileMap = TileMap.fromArray([
    [ 0, 0, 1, 1, 0, 0, 0],
    [ 0, 0, 2, 3, 0, 0, 0],
    [ 0, 0, 0, 4, 1, 0, 0],
  ]);

  let finder = new TileMapSlicer(tileMap);

  // act
  let tileMaps = finder.sliceAll();

  // assert
  assert(2, tileMaps.length);

  let tileMap1 = tileMaps.shift();
  assert(2, tileMap1.x);
  assert(0, tileMap1.y);
  assert(2, tileMap1.width);
  assert(2, tileMap1.height);

  let tileMap2 = tileMaps.shift();
  assert(3, tileMap2.x);
  assert(2, tileMap2.y);
  assert(2, tileMap2.width);
  assert(1, tileMap2.height);
});

test.run();
