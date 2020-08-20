module.exports = async function plugin(registry, opts) {
  const { enable, profilesDir } = opts;
  const engine = await registry.get('engine');

  if (enable) {
    if (!profilesDir) {
      throw new Error('profilesDir needed.');
    }

    engine.startProfiling();
  }

  return {
    onTerminate() {
      if (enable) {
        engine.stopProfiling(`${profilesDir}/profile-${Date.now()}.prof`);
      }
    }
  }
}