import { Models } from '../chunklands.node';

export interface IModelLoaderConfig {
  assetsDir: string;
}

export type ModelLoader =
  | Models.Model
  | ((config: IModelLoaderConfig) => Models.Model | Promise<Models.Model>);
export type ModelsLoader = (
  config: IModelLoaderConfig
) => Promise<Models.Model[]>;
