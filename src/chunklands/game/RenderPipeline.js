const fs = require('fs');
const { promisify } = require('util');
const engine = require('../engine');
const gl = require('../gl');
const files = require('../../files');

class RenderPipeline {
  constructor() {

  }

  /**
   * 
   * @param {{scene: import('./_').Scene, ssao: boolean, font: string}} param0 
   */
  async initialize({scene, ssao, font}) {
    this._gbufferPass = new engine.GBufferPass();
    this._gbufferPass.setProgram(await createProgram('gbuffer'));
    scene.setGBufferPass(this._gbufferPass);

    if (ssao) {
      this._ssaoPass = new engine.SSAOPass();
      this._ssaoPass.setProgram(await createProgram('ssao'));
      scene.setSSAOPass(this._ssaoPass);

      this._ssaoBlurPass = new engine.SSAOBlurPass();
      this._ssaoBlurPass.setProgram(await createProgram('ssaoblur'));
      scene.setSSAOBlurPass(this._ssaoBlurPass);
    }

    this._lightingPass = new engine.LightingPass();
    this._lightingPass.setSSAO(ssao);
    this._lightingPass.setProgram(await createProgram('lighting', {
      defines: {
        SSAO: ssao
      }
    }));
    scene.setLightingPass(this._lightingPass);

    this._skyboxPass = new engine.SkyboxPass();
    this._skyboxPass.setProgram(await createProgram('skybox'));
    scene.setSkyboxPass(this._skyboxPass);

    this._skybox = new engine.Skybox();
    this._skybox.initialize(files.skyboxDir('skybox_day'));
    scene.setSkybox(this._skybox);

    this._gameOverlayRenderer = new engine.GameOverlayRenderer();
    this._gameOverlayRenderer.setProgram(await createProgram('game_overlay'));
    scene.setGameOverlayRenderer(this._gameOverlayRenderer);

    this._blockSelectPass = new engine.BlockSelectPass();
    this._blockSelectPass.setProgram(await createProgram('block_select'));
    scene.setBlockSelectPass(this._blockSelectPass);

    this._fontLoader = new engine.FontLoader();
    const basePath = `${files.fontsDir()}/${font}`;
    const json = JSON.parse(await promisify(fs.readFile)(`${basePath}.json`));
    this._fontLoader.load(json, `${basePath}.png`);
    this._textRenderer = new engine.TextRenderer();
    this._textRenderer.setFontLoader(this._fontLoader);
    this._textRenderer.setProgram(await createProgram('text'));
    scene.setTextRenderer(this._textRenderer);
  }
}

async function createProgram(shaderName, opts) {
  const shaderDir = files.shaderDir();
  const vertexShader = `${shaderDir}/${shaderName}.vert`;
  const fragmentShader = `${shaderDir}/${shaderName}.frag`;

  return await gl.Program.create({vertexShader, fragmentShader}, opts);
}

module.exports = RenderPipeline;