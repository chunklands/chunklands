const tap = require('tap');
const sinon = require('sinon');
const {
  boundaries,
  touchingChunkCoordsWithinDistance,
  centerChunkPos,
  BiomeGenerator
} = require('./biomes');

tap.test('boundaries', async t => {
  const it = boundaries(1, -2, 3);

  function hasNext() {
    const next = it.next();
    t.false(next.done);
    return next.value;
  }

  // center
  t.deepEqual(hasNext(), [1, -2]);

  // 3x3
  // rows
  t.deepEqual(hasNext(), [0, -3]);
  t.deepEqual(hasNext(), [0, -1]);
  t.deepEqual(hasNext(), [1, -3]);
  t.deepEqual(hasNext(), [1, -1]);
  t.deepEqual(hasNext(), [2, -3]);
  t.deepEqual(hasNext(), [2, -1]);
  // cols
  t.deepEqual(hasNext(), [0, -2]);
  t.deepEqual(hasNext(), [2, -2]);

  // 5x5
  // rows
  t.deepEqual(hasNext(), [-1, -4]);
  t.deepEqual(hasNext(), [-1, 0]);
  t.deepEqual(hasNext(), [0, -4]);
  t.deepEqual(hasNext(), [0, 0]);
  t.deepEqual(hasNext(), [1, -4]);
  t.deepEqual(hasNext(), [1, 0]);
  t.deepEqual(hasNext(), [2, -4]);
  t.deepEqual(hasNext(), [2, 0]);
  t.deepEqual(hasNext(), [3, -4]);
  t.deepEqual(hasNext(), [3, 0]);
  // cols
  t.deepEqual(hasNext(), [-1, -3]);
  t.deepEqual(hasNext(), [3, -3]);
  t.deepEqual(hasNext(), [-1, -2]);
  t.deepEqual(hasNext(), [3, -2]);
  t.deepEqual(hasNext(), [-1, -1]);
  t.deepEqual(hasNext(), [3, -1]);

  t.true(it.next().done);
});

tap.test('centerChunkPos', async t => {
  t.deepEqual(centerChunkPos(0, 0, 16), [0, 0]);
  t.deepEqual(centerChunkPos(1, 0, 16), [0, 0]);
  t.deepEqual(centerChunkPos(0, 1, 16), [0, 0]);
  t.deepEqual(centerChunkPos(1, 1, 16), [0, 0]);
  t.deepEqual(centerChunkPos(-1, 0, 16), [-1, 0]);
  t.deepEqual(centerChunkPos(0, -1, 16), [0, -1]);
  t.deepEqual(centerChunkPos(-1, -1, 16), [-1, -1]);

  t.deepEqual(centerChunkPos(-16, 16, 16), [-1, 1]);
  t.deepEqual(centerChunkPos(6543, -1234, 16), [408, -78]);
});

tap.test('touchingChunkCoordsWithinDistance', async t => {
  t.matchSnapshot([...touchingChunkCoordsWithinDistance(-18, 31, 20, 16)]);
});

tap.test('BiomeGenerator #1', async t => {
  const pointGenerator = sinon.spy((x, z) => {
    if (x === 0 && z === 0) {
      return [{x: 0, z: 0, name: 'A'}];
    }

    return [];
  });

  const b = new BiomeGenerator(4, pointGenerator);

  const {nearestPoints, nearestDistances} = b.generateChunk(0, 0);
  const points = nearestPoints.map(p => p.name);
  t.matchSnapshot({
    points,
    nearestDistances,
    pointGeneratorCalls: pointGenerator.getCalls().map(c => c.args)
  });
});

tap.test('BiomeGenerator #2', async t => {
  const pointA = {x: -3, z: 5, name: 'A'};
  const pointB = {x: 1, z: 6, name: 'B'};
  const pointC = {x: 1, z: 7, name: 'C'};
  const pointGenerator = sinon.spy((x, z) => {
    switch (`${x}:${z}`) {
      case '-2:2':
        return [pointA];
      case '0:3':
        return [pointB, pointC];
    }

    return [];
  });

  const b = new BiomeGenerator(2, pointGenerator);

  const {nearestPoints, nearestDistances} = b.generateChunk(-1, 3);
  const points = nearestPoints.map(p => p.name);
  t.matchSnapshot({
    points,
    nearestDistances,
    pointGeneratorCalls: pointGenerator.getCalls().map(c => c.args)
  });
});