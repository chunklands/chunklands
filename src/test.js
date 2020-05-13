const chunklands = require('./chunklands_test');

const success = chunklands.test();
if (!success) {
  console.error('tests failed');
  process.exit(1);
}