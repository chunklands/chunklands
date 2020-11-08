import { Transform, Writable, pipeline, TransformCallback } from 'stream';
import assert from 'assert';

class LineReader extends Transform {
  private data = Buffer.from('');

  constructor() {
    super({ readableObjectMode: true });
  }

  _transform(chunk: Buffer, encoding: string, callback: TransformCallback) {
    this.data = Buffer.concat([this.data, chunk]);

    for (;;) {
      const newLineIndex = this.data.indexOf('\n');
      if (newLineIndex === -1) {
        break;
      }

      this.push(this.data.subarray(0, newLineIndex));
      this.data = this.data.subarray(newLineIndex + 1);
    }

    callback();
  }

  _flush(callback: TransformCallback) {
    if (this.data.length > 0) {
      this.push(this.data);
    }

    callback();
  }
}

class ObjFileParser extends Transform {
  static TOKEN_SPLIT = /\s+/;

  private v: number[][] = [];
  private vt: number[][] = [];
  private vn: number[][] = [];

  constructor() {
    super({ objectMode: true });
  }

  _transform(chunk: Buffer, encoding: string, callback: TransformCallback) {
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
        assert.strictEqual(v.length, 3);
        this.v.push(v);
        break;
      }
      case 'vt': {
        const vt = tokensParseNumber(tokens);
        assert.strictEqual(vt.length, 2);
        this.vt.push(vt);
        break;
      }
      case 'vn': {
        const vn = tokensParseNumber(tokens);
        assert.strictEqual(vn.length, 3);
        this.vn.push(vn);
        break;
      }
      case 'f': {
        const t0 = tokens[1].split('/');
        const t1 = tokens[2].split('/');
        const t2 = tokens[3].split('/');

        // triangle 1

        this.pushTokens(t2 as [string, string, string]);
        this.pushTokens(t1 as [string, string, string]);
        this.pushTokens(t0 as [string, string, string]);

        if (tokens.length - 1 === 4) {
          const t3 = tokens[4].split('/');

          // triangle 2
          this.pushTokens(t3 as [string, string, string]);
          this.pushTokens(t2 as [string, string, string]);
          this.pushTokens(t0 as [string, string, string]);
        }

        break;
      }
    }

    callback();
  }

  _flush(callback: TransformCallback) {
    callback();
  }

  private pushVertex(vStr: string) {
    if (!vStr) {
      throw new Error(`err in ${vStr}`);
    }

    const v = this.v[+vStr - 1];
    if (!v) {
      throw new Error(`err in ${vStr}`);
    }

    this.push(v);
  }

  private pushTokens([vStr, vtStr, vnStr]: [string, string, string]) {
    // change order to vertex, normals, uvs
    this.pushVertex(vStr);
    this.pushNormal(vnStr);
    this.pushTexture(vtStr);
  }

  private pushTexture(vtStr: string) {
    if (!vtStr) {
      throw new Error(`err in ${vtStr}`);
    }

    const vt = this.vt[+vtStr - 1];
    if (!vt) {
      throw new Error(`err in ${vtStr}`);
    }

    this.push(vt);
  }

  pushNormal(vnStr: string) {
    if (!vnStr) {
      throw new Error(`err in ${vnStr}`);
    }

    const vn = this.vn[+vnStr - 1];
    if (!vn) {
      throw new Error(`err in ${vnStr}`);
    }

    this.push(vn);
  }
}

class Sink extends Writable {
  private buffer: number[] = [];

  constructor() {
    super({ objectMode: true });
  }

  getBuffer() {
    return this.buffer;
  }

  _write(
    chunk: number[],
    encoding: string,
    callback: (error?: Error | null) => void
  ) {
    for (let i = 0; i < chunk.length; i++) {
      assert.ok(
        typeof chunk[i] === 'number',
        `chunk[i] = ${chunk[i]} (${typeof chunk[i]})`
      );
    }

    this.buffer.push(...chunk);
    callback();
  }

  _final(callback: (error?: Error | null) => void) {
    callback();
  }
}

export async function readObj(input: NodeJS.ReadableStream) {
  return new Promise<number[]>((resolve, reject) => {
    const sink = new Sink();
    pipeline(input, new LineReader(), new ObjFileParser(), sink, (err) => {
      if (err) {
        reject(err);
        return;
      }

      resolve(sink.getBuffer());
    });
  });
}

function tokensParseNumber(tokens: string[]) {
  const result = [];
  for (let i = 1; i < tokens.length; i++) {
    result.push(+tokens[i]);
  }

  return result;
}
