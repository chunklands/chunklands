interface BaseModel {
  id: string;
  texture?: Buffer;
}

interface BlockModel extends BaseModel {
  type: 'block';
  faces: { [face: string]: ArrayBuffer };
  opaque: boolean;
}

interface SpriteModel extends BaseModel {
  type: 'sprite';
  data: ArrayBuffer;
}

export type Model = BlockModel | SpriteModel;

export interface IModelLoaderConfig {
  assetsDir: string;
}

export type ModelLoader =
  | Model
  | ((config: IModelLoaderConfig) => Model | Promise<Model>);
export type ModelsLoader = (
  config: IModelLoaderConfig
) => Promise<Model[]>;
