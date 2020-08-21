module.exports = function createBatchCall() {
  let asyncCallbacks = [];
  const batchCall = {
    add(asyncCallback) {
      asyncCallbacks.push(asyncCallback);
      return batchCall;
    },
    async call(...args) {
      await Promise.all(asyncCallbacks.map(asyncCallback => asyncCallback(...args)));
      asyncCallbacks = [];
    }
  };

  return batchCall;
}