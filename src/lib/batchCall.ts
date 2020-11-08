
type AsyncCallback = (...args: any[]) => any | Promise<any>

export default function createBatchCall() {
  let asyncCallbacks: AsyncCallback[] = [];
  const batchCall = async (...args: any[]) => {
    await Promise.all(
        asyncCallbacks.map(asyncCallback => asyncCallback(...args)));
    asyncCallbacks = [];
  };

  batchCall.add = (asyncCallback: AsyncCallback) => {
    asyncCallbacks.push(asyncCallback);
    return batchCall;
  };

  return batchCall;
}
