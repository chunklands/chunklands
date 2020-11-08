declare namespace chunklands {
  export class EngineBridge {
    // TODO(daaitch): naming profilingStart(), profilingStop()
    startProfiling(): void;
    stopProfiling(outFile: string): void;

    blockCreate(model: Models.BlockModel): void;
    // TODO(daaitch): it's also spriteBake - better naming here, maybe renderPipelineBake?
    blockBake(): Promise<Block.BakeResult>;

    cameraAttachWindow(windowHandle: bigint): Promise<void>;
    cameraDetachWindow(windowHandle: bigint): Promise<void>;
    cameraGetPosition(): Promise<Math.Pos3D>;
    cameraOn(
      type: 'positionchange',
      handler: (pos: Math.Pos3D) => void
    ): Unsubscribe;

    characterIsCollision(): boolean;
    characterSetCollision(collision: boolean): void;
    characterIsFlightMode(): boolean;
    characterSetFlightMode(flightMode: boolean): void;

    chunkCreate(x: number, y: number, z: number): Promise<bigint>;
    chunkDelete(handle: bigint): Promise<void>;
    chunkUpdate(handle: bigint, data: ArrayBuffer): Promise<void>;

    fontLoad(font: Font.Font): Promise<bigint>;

    gameOn(
      type: 'pointingblockchange',
      handler: (event: { pos: Math.Pos3D | null }) => void
    ): Unsubscribe;

    GLFWInit(): Promise<void>;
    GLFWPollEvents(): void;

    renderPipelineInit(
      windowHandle: bigint,
      opts: RenderPipeline.InitOpts
    ): Promise<void>;

    sceneAddChunk(chunkHandle: bigint): Promise<void>;

    spriteCreate(model: Models.SpriteModel): void;
    spriteInstanceCreate(spriteHandle: bigint): Promise<bigint>;
    spriteInstanceUpdate(
      instanceHandle: bigint,
      update: Sprite.SpriteInstanceUpdate
    ): Promise<void>;

    terminate(): void;

    textCreate(fontHandle: bigint): Promise<bigint>;
    textUpdate(handle: bigint, update: Text.TextUpdate): Promise<void>;

    windowCreate(width: number, height: number, title: string): Promise<bigint>;
    windowGetContentSize(handle: bigint): Math.Size2D;
    windowLoadGL(handle: bigint): Promise<void>;
    windowOn(
      handle: bigint,
      type: 'shouldclose',
      handler: () => void
    ): Unsubscribe;
    windowOn(
      handle: bigint,
      type: 'click',
      handler: (event: { button: number; action: number; mods: number }) => void
    ): Unsubscribe;
    windowOn(
      handle: bigint,
      type: 'key',
      handler: (event: {
        key: number;
        scancode: number;
        action: number;
        mods: number;
      }) => void
    ): Unsubscribe;
    windowOn(
      handle: bigint,
      type: 'resize',
      handler: (event: { width: number; height: number }) => void
    ): Unsubscribe;
    windowOn(
      handle: bigint,
      type: 'contentresize',
      handler: (event: {
        width: number;
        height: number;
        xscale: number;
        yscale: number;
      }) => void
    ): Unsubscribe;
    windowOn(
      handle: bigint,
      type: 'scroll',
      handler: (event: { x: number; y: number }) => void
    ): Unsubscribe;
  }

  export namespace Block {
    interface BakeResult {
      sprites: { [id: string]: bigint };
      blocks: { [id: string]: bigint };
    }
  }

  export namespace Font {
    interface Font extends FontDefinition {
      texture: Buffer;
    }

    interface FontDefinition {
      name: string;
      size: number;
      bold: boolean;
      italic: boolean;
      width: number;
      height: number;
      characters: {
        [character: string]: {
          x: number;
          y: number;
          width: number;
          height: number;
          originX: number;
          originY: number;
          advance: number;
        };
      };
    }
  }

  export namespace Math {
    interface Pos3D {
      x: number;
      y: number;
      z: number;
    }

    interface Size2D {
      width: number;
      height: number;
    }
  }

  export namespace Models {
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

    type Model = BlockModel | SpriteModel;
  }

  export namespace RenderPipeline {
    interface InitShader {
      // TODO(daaitch): string would be a better interface than Buffer
      vertexShader: Buffer;
      fragmentShader: Buffer;
    }

    interface InitOpts {
      gbuffer: InitShader;
      lighting: InitShader;
      blockSelect: InitShader;
      sprite: InitShader;
      text: InitShader;
    }
  }

  export namespace Sprite {
    interface SpriteInstanceUpdate {
      x?: number;
      y?: number;
      z?: number;
      show?: boolean;
      scale?: number;
    }
  }

  export namespace Text {
    interface TextUpdate {
      pos?: {
        x: number;
        y: number;
      };
      text?: string;
    }
  }

  export type Unsubscribe = () => void;
}

export = chunklands;
