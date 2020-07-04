import * as gl from '../gl/_'
import { EventEmitter } from 'events'

type vec2 = [number, number]
type vec3 = [number, number, number]

/**
 * Opaque interface for the scene.
 */
export interface IScene {}

/**
 * Opaque interface for collisions.
 */
export interface ICollisionSystem {}

/**
 * Abstract class for a render pass.
 */
export declare class RenderPass {
  /**
   * Sets the program and run `virtual void InitializeProgram()`
   */
  setProgram(program: gl.ProgramBase): void
}

/**
 * Environment static object for glfw.
 */
export declare class Environment {
  /**
   * Initializes glfw.
   */
  static initialize(): void

  /**
   * Loads OpenGL proc addresses.
   * Has to be called after {@link Window#makeContextCurrent}.
   */
  static loadProcs(): void

  /**
   * Terminates glfw.
   */
  static terminate(): void
}

/**
 * Game loop implementation presenting the scene.
 */
export declare class GameLoop {
  /**
   * Sets internal state to 'started'.
   */
  start(): void

  /**
   * Sets internal state to 'stopped'.
   */
  stop(): void

  /**
   * Runs one loop. Should be called again and again.
   */
  loop(): void

  /**
   * Sets the scene to present.
   */
  setScene(scene: IScene): void
}

/**
 * G-Buffer implementation.
 */
export declare class GBufferPass extends RenderPass {}

/**
 * Lighting implementation.
 */
export declare class LightingPass extends RenderPass {
  /**
   * Enable or disable SSAO. Has to be called before {@link RenderPass#setProgram}.
   * @param ssao 
   */
  setSSAO(ssao: boolean): void
}

/**
 * Skybox pass implementation.
 */
export declare class SkyboxPass extends RenderPass {}

/**
 * SSAO implementation.
 */
export declare class SSAOPass extends RenderPass {}

/**
 * SSAO-Blur implementation.
 */
export declare class SSAOBlurPass extends RenderPass {}

export interface WindowInitialize {
  width: number
  height: number
  title: string
}

/**
 * Implementation for a glfw window.
 */
export declare class Window {
  /**
   * Initialize window.
   * @param opts Options
   */
  initialize(opts: WindowInitialize): void

  /**
   * Make this window the current context. Invoke before {@link Environment#loadProcs}.
   */
  makeContextCurrent(): void

  /**
   * Returns whether window should be closed.
   */
  shouldClose(): boolean

  /**
   * Closes the window.
   */
  close(): void

  /**
   * Returns if window is in game-control mode.
   */
  getGameControl(): boolean

  /**
   * Sets game-control mode.
   */
  setGameControl(gameControl: boolean): void

  /**
   * Public events.
   */
  events: EventEmitter
}

/**
 * Skybox.
 */
export declare class Skybox {
  initialize(texturePath: string): void
}

/**
 * Camera.
 */
export declare class Camera {
  /**
   * Sets the current camera's position.
   */
  setPosition(x: number, y: number, z: number): void

  /**
   * Returns the camera's position.
   */
  getPosition(): vec3

  /**
   * Returns camera's look angles.
   * 
   * @returns `[0]: yaw`, `[1]: pitch`
   */
  getLook(): vec2
}

/**
 * MovementController.
 */
export declare class MovementController {
  /**
   * Sets the camera.
   */
  setCamera(camera: Camera): void

  /**
   * Sets the collision system against which to check collisions.
   */
  setCollisionSystem(collisionSystem: ICollisionSystem): void
}

/**
 * asdf
 */
interface IFontLoaderMetaCharacter {
  /**
   * character's x-cordinate.
   */
  x: number

  /**
   * character's y-cordinate.
   */
  y: number

  /**
   * character's width.
   */
  width: number

  /**
   * character's height.
   */
  height: number

  /**
   * character's x-offset to the origin.
   */
  originX: number

  /**
   * character's y-offset to the origin.
   */
  originY: number

  /**
   * character's x-offset to advance from origin to next origin.
   */
  advance: number
}

/**
 * asdf
 */
interface IFontLoaderMeta {
  /**
   * Name of the font.
   */
  name: string

  /**
   * Font size.
   */
  size: number

  /**
   * `true` if bold.
   */
  bold: boolean

  /**
   * `true` if italic.
   */
  italic: boolean

  /**
   * Total width of char map.
   */
  width: number

  /**
   * Total height of char map.
   */
  height: number

  /**
   * Characters map.
   */
  characters: Record<string, IFontLoaderMetaCharacter>
}

/**
 * FontLoader.
 */
export declare class FontLoader {
  /**
   * Loads font.
   * 
   * @param meta Meta information about the font
   * @param imagePath Path to the image
   */
  load(meta: IFontLoaderMeta, imagePath: string): void
}

/**
 * TextRenderer.
 */
export declare class TextRenderer extends RenderPass {
  /**
   * Sets the font loader.
   */
  setFontLoader(loader: FontLoader): void

  /**
   * Writes text vertices to buffer.
   */
  write(text: string): void
}

/**
 * GameOverlayRenderer.
 */
export declare class GameOverlayRenderer extends RenderPass {
}

/**
 * BlockSelectPass.
 */
export declare class BlockSelectPass extends RenderPass {
}