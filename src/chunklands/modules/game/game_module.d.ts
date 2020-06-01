import * as engine from '../engine/engine_module'
import * as gl from '../gl/gl_module'

export declare class BlockRegistrarBase {
  addBlock(): void
  loadTexture(): void
  getBlockIds(): number[]
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

  setTextRenderer(renderer: engine.TextRenderer): void

  setSkybox(skybox: engine.Skybox): void

  setMovementController(controller: engine.MovementController): void
  setCamera(camera: engine.Camera): void

  getFlightMode(): boolean
  setFlightMode(flightMode: boolean): void
}

export declare class World implements engine.ICollisionSystem {
  setChunkGenerator(chunkGenerator: ChunkGenerator): void
}