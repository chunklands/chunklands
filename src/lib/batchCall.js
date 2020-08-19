module.exports = function createBatchCall() {
  let fns = [];
  const batchCall = {
    add(fn) {
      fns.push(fn);
      return batchCall;
    },
    call(...args) {
      for (let i = 0; i < fns.length; i++) {
        fns[i](...args);
      }

      fns = [];
    }
  };

  return batchCall;
}