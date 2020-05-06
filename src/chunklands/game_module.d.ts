import * as engine from './engine_module'
import * as gl from './gl_module'

export declare class BlockRegistrarBase {
  addBlock(): void
  loadTexture(): void
  getBlockIds(): void
}

export declare interface IWorldGenerator {
  generateChunk(x: number, y: number, z: number, chunkDim: number, cb: (err: Error, chunk: number[]) => void): void
}

export declare class ChunkGenerator {
  setBlockRegistrar(value: BlockRegistrarBase): void
  setWorldGenerator(value: IWorldGenerator): void
}

export declare class Scene implements engine.IScene {
  setWindow(window: Window): void
  setWorld(world: World): void

  setGBufferPass(pass: engine.GBufferPass): void
  setSSAOPass(pass: engine.SSAOPass): void
  setSSAOBlurPass(pass: engine.SSAOBlurPass): void
  setLightingPass(pass: engine.LightingPass): void
  setSkyboxPass(pass: engine.SkyboxPass): void

  setSkybox(skybox: engine.Skybox): void
}

export declare class World {
  setChunkGenerator(chunkGenerator: ChunkGenerator): void
}