module.exports = {
  cacheFile(filename) { return `${__dirname}/../cache/${filename}`; },
  fontsDir() { return `${__dirname}/game/fonts`; },
  shaderDir() { return `${__dirname}/game/shader`; },
  skyboxDir(name) { return `${__dirname}/game/skyboxes/${name}/`; },
};