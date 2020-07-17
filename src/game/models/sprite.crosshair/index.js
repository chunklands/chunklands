
const x = 1/64;

module.exports = () => ({
  id: 'sprite.crosshair',
  faces: {
    // VERTEX1         NORMAL1   UV1   VERTEX2          NORMAL2   UV2   VERTEX3          NORMAL3   UV3
    all: [
      0.5-x, 0.5+x, 0, 0, 0, -1, 0, 1, 0.5-x, 0.5-x, 0, 0, 0, -1, 0, 0, 0.5+x, 0.5-x, 0, 0, 0, -1, 1, 0,
      0.5-x, 0.5+x, 0, 0, 0, -1, 0, 1, 0.5+x, 0.5-x, 0, 0, 0, -1, 1, 0, 0.5+x, 0.5+x, 0, 0, 0, -1, 1, 1,
    ],
  },
  texture: `${__dirname}/crosshair.png`
});