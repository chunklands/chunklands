module.exports = async function plugin(registry, opts) {
  const { enable } = opts;
  const engine = await registry.get('engine');

  if (enable) {
    engine.startProfiling();
  }
}