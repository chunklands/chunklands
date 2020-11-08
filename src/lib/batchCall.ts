type AsyncCallback = (...args: unknown[]) => unknown | Promise<unknown>;

export default function createBatchCall() {
  let asyncCallbacks: AsyncCallback[] = [];
  const batchCall = async (...args: unknown[]) => {
    await Promise.all(
      asyncCallbacks.map((asyncCallback) => asyncCallback(...args))
    );
    asyncCallbacks = [];
  };

  batchCall.add = (asyncCallback: AsyncCallback) => {
    asyncCallbacks.push(asyncCallback);
    return batchCall;
  };

  return batchCall;
}
