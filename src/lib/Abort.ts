export default class Abort {
  static ABORT_ERROR = new Error('abort error');

  static isAbort(e: Error) {
    return e === Abort.ABORT_ERROR;
  }

  static catchResolver(e: Error): void {
    if (!Abort.isAbort(e)) {
      throw e;
    }
  }

  private aborted = false;
  private asyncCallbacks = new Set<AbortCallback>();

  check() {
    if (this.aborted) {
      throw Abort.ABORT_ERROR;
    }
  }

  async race<T>(uncancelablePromise: Promise<T>): Promise<T | void> {
    this.check();

    let cleanup: (() => void) | undefined;

    try {
      return await Promise.race([
        uncancelablePromise,
        new Promise<T>((resolve, reject) => {
          const abortCleanup = this.onceAbort(() => {
            reject(Abort.ABORT_ERROR);
          });

          cleanup = () => {
            abortCleanup();
            resolve(undefined as unknown as T); // no leaks
          };
        }),
      ]);
    } finally {
      if (cleanup) {
        cleanup();
      }
    }
  }

  onceAbort(asyncCallback: AbortCallback) {
    if (this.aborted) {
      let runCb = true;
      process.nextTick(() => {
        if (runCb) {
          asyncCallback(Abort.ABORT_ERROR);
        }
      });
      return () => {
        runCb = false;
      };
    }

    this.asyncCallbacks.add(asyncCallback);
    return () => this.offAbort(asyncCallback);
  }

  offAbort(asyncCallback: AbortCallback) {
    this.asyncCallbacks.delete(asyncCallback);
  }

  async abort() {
    if (!this.aborted) {
      this.aborted = true;
      const callbackPromises: Promise<unknown>[] = [];
      this.asyncCallbacks.forEach((cb) => {
        callbackPromises.push(Promise.resolve(cb(Abort.ABORT_ERROR)));
      });
      this.asyncCallbacks.clear();

      await Promise.all(callbackPromises);
    }
  }
}

export type AbortCallback = (err: Error) => unknown | Promise<unknown>;
