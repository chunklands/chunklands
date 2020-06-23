
module.exports = () => ({
  id: 'sprite.crosshair',
  faces: {
    // VERTEX1         NORMAL1          UV1         VERTEX2          NORMAL2          UV2        VERTEX3          NORMAL3          UV3
    all: [
      -8.0, 8.0, 0.0,  0.0, 0.0, -1.0,  0.0, 1.0,  -8.0, -8.0, 0.0,  0.0, 0.0, -1.0,  0.0, 0.0,  8.0, -8.0, 0.0,  0.0, 0.0, -1.0,  1.0, 0.0,
      -8.0, 8.0, 0.0,  0.0, 0.0, -1.0,  0.0, 1.0,   8.0, -8.0, 0.0,  0.0, 0.0, -1.0,  1.0, 0.0,  8.0,  8.0, 0.0,  0.0, 0.0, -1.0,  1.0, 1.0,
    ],
  },
  texture: `${__dirname}/crosshair.png`
});