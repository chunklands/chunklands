import createBatchCall from "../../lib/batchCall";
import Abort from "../../lib/Abort";
import { PluginRegistry } from "../../lib/plugin";
import { EnginePlugin } from "../engine";
import {ModelsPlugin} from '../models'
import ChunkManager from './ChunkManager'

export default async function worldGeneratorPlugin(registry: PluginRegistry) {
  const engine = await registry.get<EnginePlugin>('engine')
  const models = await registry.get<ModelsPlugin>('models')

  const abort = new Abort();

  const chunkManager = new ChunkManager(engine, models.blocks, abort);
  engine.cameraGetPosition().then(
      cameraPos => chunkManager.updatePosition(cameraPos, abort));

  const cleanup = createBatchCall()
                      .add(engine.cameraOn(
                          'positionchange',
                          (event: any) => {
                            chunkManager.updatePosition(event, abort);
                          }))
                      .add(() => abort.abort());

  return {onTerminate: cleanup};
};
