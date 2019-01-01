const {EventEmitter} = require('events');

module.exports = class Loop extends EventEmitter {
  constructor(freq) {
    super();
    this._duration = 1000 / freq;
    this._run = false;
  }

  start() {
    this._lastLoop = Date.now();
    this._run = true;
    process.nextTick(() => this._loop());
  }

  stop() {
    this._run = false;
    clearTimeout(this._loopTimeout);
  }

  _loop() {
    const now = Date.now();
    this.emit('loop', now - this._lastLoop);
    this._lastLoop = now;

    const nowAfterLoop = Date.now();

    if (this._run) {
      this._loopTimeout = setTimeout(() => this._loop(), nowAfterLoop - now + this._duration);
    }
  };
}