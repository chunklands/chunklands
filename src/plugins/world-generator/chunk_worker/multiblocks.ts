import assert from 'assert'

export default function createMultiblocks(chunkDim: number, blocks: {[id: string]: bigint}) {
  const {
    'block.wood': BLOCK_WOOD,
    'block.air': BLOCK_AIR,
    'block.cobblestone': BLOCK_COBBLESTONE
  } = blocks;

  class MultiBlock {

    private blocks: BigUint64Array
    
    constructor(public sx: number, public sy: number, public sz: number) {
      this.blocks = new BigUint64Array(sx * sy * sz);
    }

    setBlocks(blockArray: bigint[]) {
      assert(blockArray.length === this.blocks.length);

      for (let i = 0; i < blockArray.length; i++) {
        this.blocks[i] = blockArray[i];
      }
    }

    blockAt(x: number, y: number, z: number) {
      assert(0 <= x && x < this.sx);
      assert(0 <= y && y < this.sy);
      assert(0 <= z && z < this.sz);

      const blockIndex = x + this.sx * (y + this.sy * z);
      return this.blocks[blockIndex];
    }
  }

  const tree = new MultiBlock(3, 5, 3);
  tree.setBlocks(
      blockChars({' ': BLOCK_AIR, 'w': BLOCK_WOOD, 'c': BLOCK_COBBLESTONE}, [
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
  house.setBlocks(
      blockChars({'c': BLOCK_COBBLESTONE, ' ': BLOCK_AIR, 'w': BLOCK_WOOD}, [
        '  w  ', ' www ', 'wwwww', 'wcccw', 'wcccw',

        '  w  ', ' w w ', 'w   w', '    c', 'c   c',

        '  w  ', ' w w ', 'w   w', 'c   c', 'c   c',

        '  w  ', ' w w ', 'w   w', '    c', '    c',

        '  w  ', ' w w ', 'w   w', 'c   c', 'c   c',

        '  w  ', ' www ', 'wwwww', 'wcccw', 'wcccw',
      ]));

  function blockChars(blocks: {[c: string]: bigint}, inputArr: string[]) {
    const buf = [];
    for (let i = inputArr.length - 1; i >= 0; i--) {
      const input = inputArr[i];
      for (const c of input) {
        assert(c in blocks);
        buf.push(blocks[c]);
      }
    }

    return buf;
  }

  class MultiBlockRef {

    constructor(private multiblock: MultiBlock, private x: number, private y: number, private z: number) {
    }

    touchesChunk(chunkX: number, chunkY: number, chunkZ: number) {
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

  return {tree, house, MultiBlockRef};
}
