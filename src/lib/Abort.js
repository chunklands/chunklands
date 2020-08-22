
class Abort {
  static ABORT_ERROR = new Error('abort error');

  static isAbort(e) {
    return e === Abort.ABORT_ERROR;
  }

  static catchResolver(e) {
    if (!Abort.isAbort(e)) {
      throw e;
    }
  }

  constructor() {
    this.aborted = false;
    this._asyncCallbacks = new Set();
  }

  check() {
    if (this.aborted) {
      throw Abort.ABORT_ERROR;
    }
  }

  async race(uncancelablePromise) {
    this.check();

    let cleanup;

    try {
      return await Promise.race([
        uncancelablePromise,
        new Promise((resolve, reject) => {
          const abortCleanup = this.onceAbort(() => {
            reject(Abort.ABORT_ERROR);
          });

          cleanup = () => {
            abortCleanup();
            resolve(); // no leaks
          };
        })
      ]);
    } finally {
      if (cleanup) {
        cleanup();
      }
    }
  }

  onceAbort(asyncCallback) {
    if (this.aborted) {
      let runCb = true;
      process.nextTick(() => {
        if (runCb) {
          asyncCallback(ABORT_ERROR);
        }
      });
      return () => {
        runCb = false;
      };
    }

    this._asyncCallbacks.add(asyncCallback);
    return () => this.offAbort(asyncCallback);
  }

  offAbort(cb) {
    this._asyncCallbacks.delete(cb);
  }
  
  async abort() {
    if (!this.aborted) {
      this.aborted = true;
      const callbackPromises = []
      this._asyncCallbacks.forEach(cb => { 
        callbackPromises.push(Promise.resolve(cb(Abort.ABORT_ERROR)));
      });
      this._asyncCallbacks.clear();

      await Promise.all(callbackPromises);
    }
  }
}

module.exports = Abort;