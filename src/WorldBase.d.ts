import {SkyboxPassBase} from './SkyboxPassBase'
import {ChunkGeneratorBase} from './ChunkGeneratorBase'
import {GBufferPassBase} from './GBufferPassBase'
import {SSAOBlurPassBase} from './SSAOBlurPassBase'
import {SSAOPassBase} from './SSAOPassBase'
import {SkyboxBase} from './SkyboxBase'
import {LightingPassBase} from './LightingPassBase'
export {WorldBase}


declare class WorldBase {
  setChunkGenerator(chunkGenerator: ChunkGeneratorBase): void
  setSkybox(skybox: SkyboxBase): void

  setSkyboxPass(pass: SkyboxPassBase): void
  setLightingPass(pass: LightingPassBase): void
}
