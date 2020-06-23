const models = [
  require('./block.air'),
  require('./block.coal'),
  require('./block.cobblestone'),
  require('./block.dirt'),
  require('./block.gold'),
  require('./block.grass'),
  require('./block.iron'),
  require('./block.monkey'),
  require('./block.sand'),
  require('./block.stone'),
  require('./block.water'),
  require('./block.wood'),
  require('./sprite.crosshair'),
];

module.exports = () => Promise.all(models.map(createFn => createFn()));
