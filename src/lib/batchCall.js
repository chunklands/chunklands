module.exports = function createBatchCall() {
  let asyncCallbacks = [];
  const batchCall = async (...args) => {
    await Promise.all(
        asyncCallbacks.map(asyncCallback => asyncCallback(...args)));
    asyncCallbacks = [];
  };

  batchCall.add = asyncCallback => {
    asyncCallbacks.push(asyncCallback);
    return batchCall;
  };

  return batchCall;
}