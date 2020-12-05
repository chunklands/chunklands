import {Engine, Log, Window, Renderpipeline} from 'crankshaft-node-binding'

Log.setLevel(1);

(async () => {
  const engine = new Engine();
  await engine.init();

  const window = await Window.create(engine, {
    width: 1024,
    height: 768,
    title: "CRANKSHAFT"
  });

  await window.loadGL();
  // await Renderpipeline.init(window.handle, {});

  window.onClose(async () => {
    await window.destroy();
    await engine.stop();
    engine.destroy();
  })
})()
