const stream = require('stream');
const assert = require('assert');

class LineReader extends stream.Transform {
  constructor() {
    super({readableObjectMode: true});
    this._data = Buffer.from('');
  }

  /**
   * @param {Buffer} chunk
   * @param {string} encoding
   * @param {Function} callback
   */
  _transform(chunk, encoding, callback) {
    this._data = Buffer.concat([this._data, chunk]);

    while (true) {
      const newLineIndex = this._data.indexOf('\n');
      if (newLineIndex === -1) {
        break;
      }

      this.push(this._data.subarray(0, newLineIndex));
      this._data = this._data.subarray(newLineIndex + 1);
    }

    callback();
  }

  _flush(callback) {
    if (this._data.length > 0) {
      this.push(this._data);
    }

    callback();
  }
}

class ObjFileParser extends stream.Transform {
  static TOKEN_SPLIT = /\s+/;
  constructor() {
    super({objectMode: true});

    this._v = [];
    this._vt = [];
    this._vn = [];
  }

  /**
   * @param {Buffer} chunk
   * @param {string} encoding
   * @param {Function} callback
   */
  _transform(chunk, encoding, callback) {
    const str = chunk.toString().trim();

    if (!str) {
      callback();
      return;
    }

    if (str.startsWith('#')) {
      callback();
      return;
    }

    const tokens = str.split(ObjFileParser.TOKEN_SPLIT);
    switch (tokens[0]) {
      case 'v': {
        const v = tokensParseNumber(tokens);
        assert.equal(v.length, 3);
        this._v.push(v);
        break;
      }
      case 'vt': {
        const vt = tokensParseNumber(tokens);
        assert.equal(vt.length, 2);
        this._vt.push(vt);
        break;
      }
      case 'vn': {
        const vn = tokensParseNumber(tokens);
        assert.equal(vn.length, 3);
        this._vn.push(vn);
        break;
      }
      case 'f': {
        const t0 = tokens[1].split('/');
        const t1 = tokens[2].split('/');
        const t2 = tokens[3].split('/');

        // triangle 1

        this._pushTokens(t2);
        this._pushTokens(t1);
        this._pushTokens(t0);

        if (tokens.length - 1 === 4) {
          const t3 = tokens[4].split('/');

          // triangle 2
          this._pushTokens(t3);
          this._pushTokens(t2);
          this._pushTokens(t0);
        }

        break;
      }
    }

    callback();
  }

  _flush(callback) {
    callback();
  }

  /**
   * @param {string} vStr
   */
  _pushVertex(vStr) {
    if (!vStr) {
      throw new Error(`err in ${vStr}`)
    }

    const v = this._v[(+vStr) - 1];
    if (!v) {
      throw new Error(`err in ${vStr}`)
    }

    this.push(v);
  }

  /**
   *
   * @param {[string, string, string]} param0
   */
  _pushTokens([vStr, vtStr, vnStr]) {
    // change order to vertex, normals, uvs
    this._pushVertex(vStr);
    this._pushNormal(vnStr);
    this._pushTexture(vtStr);
  }

  /**
   * @param {string} vtStr
   */
  _pushTexture(vtStr) {
    if (!vtStr) {
      throw new Error(`err in ${vtStr}`)
    }

    const vt = this._vt[(+vtStr) - 1];
    if (!vt) {
      throw new Error(`err in ${vtStr}`)
    }

    this.push(vt);
  }

  /**
   * @param {string} vnStr
   */
  _pushNormal(vnStr) {
    if (!vnStr) {
      throw new Error(`err in ${vnStr}`)
    }

    const vn = this._vn[(+vnStr) - 1];
    if (!vn) {
      throw new Error(`err in ${vnStr}`)
    }

    this.push(vn);
  }
}

class Sink extends stream.Writable {
  constructor() {
    super({objectMode: true});

    /** @type {number[]} */
    this._buffer = [];
  }

  getBuffer() {
    return this._buffer;
  }

  _write(chunk, encoding, callback) {
    for (let i = 0; i < chunk.length; i++) {
      assert.ok(
          typeof chunk[i] === 'number',
          `chunk[i] = ${chunk[i]} (${typeof chunk[i]})`);
    }

    this._buffer.push(...chunk);
    callback();
  }

  _final(callback) {
    callback();
  }
}

/**
 * @param {NodeJS.ReadableStream} input
 * @return {Promise<number[]>}
 */
function readObj(input) {
  return new Promise((resolve, reject) => {
    const sink = new Sink();
    stream.pipeline(input, new LineReader(), new ObjFileParser(), sink, err => {
      if (err) {
        reject(err);
        return;
      }

      resolve(sink.getBuffer());
    });
  });
}

/**
 * @param {string[]} tokens
 */
function tokensParseNumber(tokens) {
  const result = [];
  for (let i = 1; i < tokens.length; i++) {
    result.push(+tokens[i]);
  }

  return result;
}

module.exports = readObj;