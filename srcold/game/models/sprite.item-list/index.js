
const n = 9;
const screenPaddingX = 1/4;
const screenPaddingBottom = 1/128;
const itemPaddingX = 1/256;

const itemSize = (1 - (2*screenPaddingX) - ((n-1)*itemPaddingX)) / n;
function itemLeft(i) {
  return screenPaddingX + i * (itemSize + itemPaddingX);
}

function data(i) {
  const l = itemLeft(i);
  const r = l + itemSize;
  const b = screenPaddingBottom;
  const t = b + itemSize;

  return [
    l, t, 0, 0, 0, -1, 0, 1,
    l, b, 0, 0, 0, -1, 0, 0,
    r, b, 0, 0, 0, -1, 1, 0,

    l, t, 0, 0, 0, -1, 0, 1,
    r, b, 0, 0, 0, -1, 1, 0,
    r, t, 0, 0, 0, -1, 1, 1,
  ];
}

module.exports = () => ({
  id: 'sprite.item-list',
  faces: {
    all: [
      ...data(0),
      ...data(1),
      ...data(2),
      ...data(3),
      ...data(4),
      ...data(5),
      ...data(6),
      ...data(7),
      ...data(8),
    ],
  },
  texture: `${__dirname}/item.png`
});