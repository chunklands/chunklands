const assert = require('assert');

function create(chunkDim, blocks) {

  const { 'block.wood': BLOCK_WOOD, 'block.air': BLOCK_AIR, 'block.cobblestone': BLOCK_COBBLESTONE } = blocks;

  class MultiBlock {
    /**
     * @param {number} sx 
     * @param {number} sy 
     * @param {number} sz 
     */
    constructor(sx, sy, sz) {
      this.sx = sx;
      this.sy = sy;
      this.sz = sz;

      this.blocks = new Int32Array(sx * sy * sz);
    }

    /**
     * @param {number[]} blockArray 
     */
    setBlocks(blockArray) {
      assert(blockArray.length === this.blocks.length);

      for (let i = 0; i < blockArray.length; i++) {
        this.blocks[i] = blockArray[i];
      }
    }

    blockAt(x, y, z) {
      if (!(0 <= z && z < this.sz)) {
        console.log({z})
      }
      assert(0 <= x && x < this.sx);
      assert(0 <= y && y < this.sy);
      assert(0 <= z && z < this.sz);

      const blockIndex = x + this.sx * (y + this.sy * z);
      return this.blocks[blockIndex];
    }
  }

  const tree = new MultiBlock(3, 5, 3);
  tree.setBlocks(blockChars({
    ' ': BLOCK_AIR,
    'w': BLOCK_WOOD,
    'c': BLOCK_COBBLESTONE
  }, [
    '   ',
    ' c ',
    '   ',
    '   ',
    '   ',

    ' c ',
    'ccc',
    ' c ',
    ' w ',
    ' w ',

    '   ',
    ' c ',
    '   ',
    '   ',
    '   ',

  ]));

  const house = new MultiBlock(5, 5, 6);
  house.setBlocks(blockChars({
    'c': BLOCK_COBBLESTONE,
    ' ': BLOCK_AIR,
    'w': BLOCK_WOOD
  }, [
    '  w  ',
    ' www ',
    'wwwww',
    'wcccw',
    'wcccw',

    '  w  ',
    ' w w ',
    'w   w',
    '    c',
    'c   c',

    '  w  ',
    ' w w ',
    'w   w',
    'c   c',
    'c   c',

    '  w  ',
    ' w w ',
    'w   w',
    '    c',
    '    c',

    '  w  ',
    ' w w ',
    'w   w',
    'c   c',
    'c   c',

    '  w  ',
    ' www ',
    'wwwww',
    'wcccw',
    'wcccw',
  ]));

  /**
   * @param {{[c: string]: number}} blocks 
   * @param {string[]} inputArr 
   */
  function blockChars(blocks, inputArr) {
    const buf = [];
    for (let i = inputArr.length - 1; i >= 0; i--) {
      const input = inputArr[i];
      for (const c of input) {
        assert(c in blocks);
        buf.push(blocks[c]);
      }
    }
    console.log({buf})
    return buf;
  }
  
  class MultiBlockRef {
    /**
     * @param {MultiBlock} multiblock 
     * @param {number} x 
     * @param {number} y 
     * @param {number} z 
     */
    constructor(multiblock, x, y, z) {
      this.multiblock = multiblock;
      this.x = x;
      this.y = y;
      this.z = z;
    }

    touchesChunk(chunkX, chunkY, chunkZ) {
      const offsetX = chunkX * chunkDim;
      const offsetY = chunkY * chunkDim;
      const offsetZ = chunkZ * chunkDim;

      if (this.x > offsetX + chunkDim) {
        return false;
      }

      if (this.x + this.multiblock.sx < offsetX) {
        return false;
      }

      if (this.y > offsetY + chunkDim) {
        return false;
      }

      if (this.y + this.multiblock.sy < offsetY) {
        return false;
      }

      if (this.z > offsetZ + chunkDim) {
        return false;
      }

      if (this.z + this.multiblock.sz < offsetZ) {
        return false;
      }

      return true;
    }
  }

  return {
    tree,
    house,
    MultiBlockRef
  };
}

module.exports = create;
