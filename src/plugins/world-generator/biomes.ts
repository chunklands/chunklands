const assert = require('assert');

const X = 0;
const Z = 1;

export class BiomeGenerator {
  private chunks = new Map()

  constructor(private chunkSize: number, private pointGenerator) {
  }

  generateChunk(x: number, z: number) {
    const nearestPoints = new Array(this.chunkSize ** 2);
    const nearestDistances = new Array(this.chunkSize ** 2);

    const points = this.findAllRelevantPointsForChunk(x, z);
    assert(points.length > 0);

    const coffx = x * this.chunkSize;
    const coffz = z * this.chunkSize;

    let i = 0;
    for (let z = 0; z < this.chunkSize; z++) {
      for (let x = 0; x < this.chunkSize; x++) {
        const px = coffx + x;
        const pz = coffz + z;

        let nearestPoint = points[0];
        let nearestDistance =
            distance(nearestPoint.x - px, nearestPoint.z - pz);

        for (let pi = 1; pi < points.length; pi++) {
          const point = points[pi];
          const pointDistance = distance(point.x - px, point.z - pz);

          if (pointDistance < nearestDistance) {
            nearestPoint = point;
            nearestDistance = pointDistance;
          }
        }

        nearestPoints[i] = nearestPoint;
        nearestDistances[i] = nearestDistance;
        i++;
      }
    }

    return {nearestPoints, nearestDistances};
  }

  private findAllRelevantPointsForChunk(x: number, z: number) {
    const nearPoint = this.findSomeNearPointForChunk(x, z);
    const {farestDistance, farestPoint} = findFarestChunkPointResult(
        x, z, this.chunkSize, nearPoint.x, nearPoint.z);

    const points = [];
    for (const [cx, cz] of touchingChunkCoordsWithinDistance(
             farestPoint[X], farestPoint[Z], farestDistance * 2,
             this.chunkSize)) {
      const chunk = this.needChunkWithPoints(cx, cz);
      points.push(...chunk.points);
    }

    return points;
  }

  private findSomeNearPointForChunk(x: number, z: number) {
    for (const [cx, cz] of boundaries(x, z)) {
      const chunk = this.needChunkWithPoints(cx, cz);
      if (chunk.points.length > 0) {
        return chunk.points[0];
      }
    }

    throw new Error('boundary overflow');
  }

  private needChunkWithPoints(x: number, z: number) {
    const chunkKey = this.chunkKey(x, z);
    let chunk = this.chunks.get(chunkKey);
    if (chunk) {
      return chunk;
    }

    chunk = this.generateChunkInternal(x, z);
    this.chunks.set(chunkKey, chunk);

    return chunk;
  }

  private generateChunkInternal(x: number, z: number) {
    const points = this.pointGenerator(x, z);
    const chunk = {points};

    return chunk;
  }

  private chunkKey(x: number, z: number) {
    return `${x}:${z}`;
  }
}

function findFarestChunkPointResult(x: number, z: number, chunkSize: number, px: number, pz: number) {
  const f00 = [x * chunkSize, z * chunkSize];

  let farestPoint = f00;
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

export function centerChunkPos(px: number, pz: number, chunkSize: number) {
  return [Math.floor(px / chunkSize), Math.floor(pz / chunkSize)];
}

export function* touchingChunkCoordsWithinDistance(px: number, pz: number, distance: number, chunkSize: number) {
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

export function* boundaries(x: number, z: number, iterationMax = 100) {
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

function distance(dx: number, dz: number) {
  return Math.sqrt(dx ** 2 + dz ** 2);
}
