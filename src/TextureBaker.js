const SLICE_V = 1, SLICE_H = 2;

module.exports = class TextureBaker {
  constructor() {
    this._root = {x: 0, y: 0};
    this.maxDim = 0;
  }

  addArea(w, h) {
    let area = this._root;

    while (true) {
      if (!('w' in area)) {
        area.w = w;
        area.h = h;

        this.maxDim = Math.max(this.maxDim, area.x + area.w, area.y + area.h);
        return area;
      }
  
      if (area.slice === SLICE_V) {

        if (w <= area.w) {
          area = addAndReturnDown(area);
        } else {
          area = addAndReturnRight(area);
        }

      } else if (area.slice === SLICE_H) {

        if (h <= area.h) {
          area = addAndReturnRight(area);
        } else {
          area = addAndReturnDown(area);
        }

      } else { // no slice

        // we want texture to grow x as well as y
        if (area.x > area.y) { // add down, if wider than hight
          area.slice = w <= area.w ? SLICE_V : SLICE_H; // if w fits in area.w, we can slice vertically
          area = addAndReturnDown(area);
        } else { // add right, if higher (or eq) than width
          area.slice = h <= area.h ? SLICE_H : SLICE_V; // if h fits in area.h, we can slice horizontally
          area = addAndReturnRight(area);
        }
      }
    }
  }
};

function addAndReturnDown(area) {
  if (!area.down) {
    area.down = {
      x: area.x,
      y: area.y + area.h
    };
  }

  return area.down;
}

function addAndReturnRight(area) {
  if (!area.right) {
    area.right = {
      x: area.x + area.w,
      y: area.y
    };
  }

  return area.right;
}
