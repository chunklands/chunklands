import { perlin2, perlin3 } from './noise';

const PRNG_OFF = 1 / Math.PI;
const PRNG_MUL = 1 / Math.E / Math.E / Math.E / Math.E / Math.E;

/**
 * 2D pseudo random number generator.
 * @returns Normalized value [0; 1]
 */
export function prng2(
  x: number,
  y: number,
  valueOffset: number = PRNG_OFF,
  valueMult: number = PRNG_MUL
) {
  return (
    (1 + perlin2(valueOffset + x * valueMult, valueOffset + y * valueMult)) / 2
  );
}

/**
 * 3D pseudo random number generator.
 * @returns Normalized value [0; 1]
 */
export function prng3(
  x: number,
  y: number,
  z: number,
  seedOff = PRNG_OFF,
  seedMul = PRNG_MUL
) {
  return (
    (1 +
      perlin3(
        seedOff + x * seedMul,
        seedOff + y * seedMul,
        seedOff + z * seedMul
      )) /
    2
  );
}
