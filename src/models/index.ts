import blockAir from './block.air';
import blockCoal from './block.coal';
import blockCobblestone from './block.cobblestone';
import blockDirt from './block.dirt';
import blockGold from './block.gold';
import blockGrass from './block.grass';
import blockIron from './block.iron';
import blockMonkey from './block.monkey';
import blockPickaxe from './block.pickaxe';
import blockSand from './block.sand';
import blockStone from './block.stone';
import blockWater from './block.water';
import blockWood from './block.wood';
import spriteCrosshair from './sprite.crosshair';
import spriteItemlistItem from './sprite.itemlist-item';
import spriteItemlistSelection from './sprite.itemlist-selection';
import { IModelLoaderConfig, ModelLoader } from './types';

const models: ModelLoader[] = [
  blockAir,
  blockCoal,
  blockCobblestone,
  blockDirt,
  blockGold,
  blockGrass,
  blockIron,
  blockMonkey,
  blockSand,
  blockStone,
  blockWater,
  blockWood,
  blockPickaxe,
  spriteCrosshair,
  spriteItemlistItem,
  spriteItemlistSelection,
];

async function loadModels(config: IModelLoaderConfig) {
  return Promise.all(
    models.map((loader) => {
      return typeof loader !== 'function' ? loader : loader(config);
    })
  );
}

export default loadModels;
