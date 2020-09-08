
const createBatchCall = require('../lib/batchCall')

module.exports = async function plugin(registry) {
  const [] = await Promise.all([]);

  const cleanup = createBatchCall().add(() => {});

  return {
    onTerminate: cleanup
  }
}
