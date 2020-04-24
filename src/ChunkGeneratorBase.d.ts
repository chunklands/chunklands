import { BlockRegistrarBase } from "./BlockRegistrarBase";

export {ChunkGeneratorBase, IWorldGenerator}

declare interface IWorldGenerator {
  generateChunk(x: number, y: number, z: number, chunkDim: number, cb: (err: Error, chunk: number[]) => void): void
}

declare class ChunkGeneratorBase {
  setBlockRegistrar(value: BlockRegistrarBase): void
  setWorldGenerator(value: IWorldGenerator): void
}
