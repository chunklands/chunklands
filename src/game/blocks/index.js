const blocks = [
  require('./coal'),
  require('./cobblestone'),
  require('./dirt'),
  require('./gold'),
  require('./grass'),
  require('./iron'),
  require('./monkey'),
  require('./sand'),
  require('./stone'),
  require('./water'),
  require('./wood'),
  require('./air'),
];

module.exports = () => Promise.all(blocks.map(createFn => createFn()));
