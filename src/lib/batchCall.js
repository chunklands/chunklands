module.exports = function createBatchCall() {
  let fns = [];
  const batchCall = {
    add(fn) {
      fns.push(fn);
      return batchCall;
    },
    async call(...args) {
      await Promise.all(fns.map(fn => fn(...args)));
      fns = [];
    }
  };

  return batchCall;
}