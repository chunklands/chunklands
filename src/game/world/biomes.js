const assert = require('assert');

const X = 0;
const Z = 1;

class BiomeGenerator {
  constructor(chunkSize, pointGenerator) {
    this._chunkSize = chunkSize;
    this._pointGenerator = pointGenerator;

    this._chunks = new Map();
  }

  generateChunk(x, z) {
    const nearestPoints     = new Array(this._chunkSize * this._chunkSize);
    const nearestDistances  = new Array(this._chunkSize * this._chunkSize);

    const points = this._findAllRelevantPointsForChunk(x, z);
    assert(points.length > 0);

    const coffx = x * this._chunkSize;
    const coffz = z * this._chunkSize;

    let i = 0;
    for (let z = 0; z < this._chunkSize; z++) {
      for (let x = 0; x < this._chunkSize; x++) {
        const px = coffx + x;
        const pz = coffz + z;

        let nearestPoint = points[0];
        let nearestDistance = distance(nearestPoint.x - px, nearestPoint.z - pz);

        for (let pi = 1; pi < points.length; pi++) {
          const point = points[pi];
          const pointDistance = distance(point.x - px, point.z - pz);

          if (pointDistance < nearestDistance) {
            nearestPoint = point;
            nearestDistance = pointDistance;
          }
        }

        nearestPoints[i]    = nearestPoint;
        nearestDistances[i] = nearestDistance
        i++;
      }
    }

    return {nearestPoints, nearestDistances};
  }

  _findAllRelevantPointsForChunk(x, z) {
    const nearPoint = this._findSomeNearPointForChunk(x, z);
    const {farestDistance, farestPoint} = findFarestChunkPointResult(x, z, this._chunkSize, nearPoint.x, nearPoint.z);

    const points = [];
    for (const [cx, cz] of touchingChunkCoordsWithinDistance(farestPoint[X], farestPoint[Z], farestDistance * 2, this._chunkSize)) {
      const chunk = this._needChunkWithPoints(cx, cz);
      points.push(...chunk.points);
    }

    return points;
  }

  _findSomeNearPointForChunk(x, z) {
    for (const [cx, cz] of boundaries(x, z)) {
      const chunk = this._needChunkWithPoints(cx, cz);
      if (chunk.points.length > 0) {
        return chunk.points[0];
      }
    }

    throw new Error('boundary overflow');
  }

  _needChunkWithPoints(x, z) {
    const chunkKey = this._chunkKey(x, z);
    let chunk = this._chunks.get(chunkKey);
    if (chunk) {
      return chunk;
    }

    chunk = this._generateChunk(x, z);
    this._chunks.set(chunkKey, chunk);
    
    return chunk;
  }

  _generateChunk(x, z) {
    const points = this._pointGenerator(x, z);
    const chunk = {
      points
    };

    return chunk;
  }

  _chunkKey(x, z) {
    return `${x}:${z}`;
  }
}

function findFarestChunkPointResult(x, z, chunkSize, px, pz) {
  const f00 = [x * chunkSize, z * chunkSize];

  let farestPoint    = f00;
  let farestDistance = distance(f00[X] - px, f00[Z] - pz);

  {
    const f10 = [f00[X] + chunkSize - 1, f00[Z]];
    const d10 = distance(f10[X] - px, f10[Z] - pz);
    if (d10 > farestDistance) {
      farestPoint = f10;
      farestDistance = d10;
    }
  }

  {
    const f01 = [f00[X], f00[Z] + chunkSize - 1];
    const d01 = distance(f01[X] - px, f01[Z] - pz);
    if (d01 > farestDistance) {
      farestPoint = f01;
      farestDistance = d01;
    }
  }

  {
    const f11 = [f00[X] + chunkSize - 1, f00[Z] + chunkSize - 1];
    const d11 = distance(f11[X] - px, f11[Z] - pz);
    if (d11 > farestDistance) {
      farestPoint = f11;
      farestDistance = d11;
    }
  }

  return {farestPoint, farestDistance};
}

function centerChunkPos(px, pz, chunkSize) {
  return [Math.floor(px / chunkSize), Math.floor(pz / chunkSize)];
}

function* touchingChunkCoordsWithinDistance(px, pz, distance, chunkSize) {
  if (distance < 0) {
    throw new TypeError('distance should be positive');
  }

  const c0 = centerChunkPos(px - distance, pz - distance, chunkSize);
  const c1 = centerChunkPos(px + distance, pz + distance, chunkSize);

  for (let z = c0[Z]; z <= c1[Z]; z++) {
    for (let x = c0[X]; x <= c1[X]; x++) {
      yield [x, z];
    }
  }
}

function* boundaries(x, z, iterationMax = 100) {
  yield [x, z];

  for (let i = 1; i < iterationMax; i++) {
    // rows
    // xxx
    // o.o
    // xxx
    for (let fx = -i; fx <= i; fx++) {
      yield [x + fx, z - i];
      yield [x + fx, z + i];
    }

    // cols (without corners)
    // ooo
    // x.x
    // ooo
    for (let fz = -i + 1; fz <= i - 1; fz++) {
      yield [x - i, z + fz];
      yield [x + i, z + fz];
    }
  }
}

function distance(dx, dz) {
  return Math.sqrt(dx*dx + dz*dz);
}

module.exports = {
  BiomeGenerator,
  boundaries,
  touchingChunkCoordsWithinDistance,
  centerChunkPos
};