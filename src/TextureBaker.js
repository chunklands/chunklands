
module.exports = class TextureBaker {
  constructor() {
    this._root = {x: 0, y: 0, maxW: 4096};
    this.maxDim = 0;
  }

  addArea(w, h) {
    const area = this._addArea(this._root, w, h);
    if (!area) {
      throw new Error(`could not add w=${w}, h=${h}.`);
    }

    return area;
  }

  _addArea(area, w, h) {
    if (!('w' in area)) {
      area.w = w;
      area.h = h;

      this.maxDim = Math.max(this.maxDim, area.x + area.w, area.y + area.h);

      return area;
    }

    // try add down
    if (area.w >= w) {
      if (!area.down) {
        area.down = {
          x: area.x,
          y: area.y + area.h,
          maxW: area.maxW
        }
      }

      const target = this._addArea(area.down, w, h);
      if (target) {
        return target;
      }
    }
    
    // try add right
    const rightSpace = area.maxW - area.w;

    if (rightSpace >= w) {
      if (!area.right) {
        area.right = {
          x: area.x + area.w,
          y: area.y,
          maxW: rightSpace
        };
      }

      const target = this._addArea(area.right, w, h);
      if (target) {
        return target;
      }
    }

    // adding failed
    return null;
  }
};
