import {SkyboxPassBase} from './SkyboxPassBase'
import {ChunkGeneratorBase} from './ChunkGeneratorBase'
import {SkyboxBase} from './SkyboxBase'
export {WorldBase}


declare class WorldBase {
  setChunkGenerator(chunkGenerator: ChunkGeneratorBase): void
  setSkybox(skybox: SkyboxBase): void

  setSkyboxPass(pass: SkyboxPassBase): void
}
