const noise = require('../../noise');

module.exports = {
  isGround(x, y, z) {
    const p = noise.perlin3(x / 20, y / 10, z / 20);
    const np = (p+1) / 2;
    const v = np*np*np * (1-lerpInverse(-20, 20, y));
    return v > 0.15;
  }
  // isGround: transition(-20, 20, underground, aboveground)
};



function transition(ya, yb, fa, fb) {
  return (x, y, z) => {
    if (y >= yb) {
      return fb(x, y, z);
    }

    if (y <= ya) {
      return fa(x, y, z);
    }

    const va = fa(x, y, z); // [0; 1]
    const vb = fb(x, y, z); // [0; 1]

    const f = lerpInverse(ya, yb, y); // [0; 1]

    const v = (f * vb) + ((1 - f) * va); // [0; 1]
    assertN(v);

    return v >= 0.5;
  };
}

function aboveground(x, y, z) {
  const cc = crazyCanyon(x, y, z);
  const f = 1 - lerpInverse(0, 50, y)
  const v = cc*cc*cc * f*f*f;
  
  assertN(v);
  return v;
}

function underground(x, y, z) {
  const v = caveCanyon(x, y, z);
  
  assertN(v);
  return v;
}

// function isGroundV1(x, y, z) {
//   const v = noise.perlin3(x / 40, y / 20, z / 40);

//   return v * 200 > y;
// }

function caveCanyon(x, y, z) {
  const p = noise.perlin3(x / 30, y / 50, z / 30);    // [-1;1]
  const v = 200 * p*p*p;                              // [-200;200]

  const r = lerpInverseLC(0, y, v);                   // [0; 1]

  assertN(r);
  return r;
}

// function isCaveSystem(x, y, z) {
//   const v = noise.perlin3(x / 30, y / 10, z / 30);
//   return v*v*v*v*v * 1000 > y;
// }

function smoothCanyon(x, y, z) {
  const p = noise.perlin3(x / 40, y / 20, z / 40);    // [-1;1]
  const v = 200 * p*p*p*p*p;                          // [-200;200]
  const r = lerpInverseLC(0, y, v);                   // [0; 1]
  
  assertN(r);
  return r;
}

function crazyCanyon(x, y, z) {
  const p = noise.perlin3(x / 20, y / 10, z / 20);    // [-1;1]
  const v = 200 * p;                                  // [-200;200]
  const r = lerpInverseLC(0, y, v);                   // [0; 1]
  assertN(r);
  return r;
}

function lerp(a, b, t) {
  if (t <= 0) {
    return a;
  }

  if (t >= 1) {
    return b;
  }

  const v = a + t * (b-a);
  assert(v);
  return v;
}

function lerpInverse(a, b, t) {
  if (t <= a) {
    return 0;
  }

  if (t >= b) {
    return 1;
  }

  const v = (t - a) / (b - a);
  assertN(v);
  return v;
}

function lerpInverseLC(a, b, t) {
  const v = a < b ? lerpInverse(a, b, t) : lerpInverse(b, a, t);
  assertN(v);
  return v;
}

function assert(cond) {
  if (!cond) {
    throw new TypeError('condition failed');
  }
}

function assertN(x) {
  assert(x >= 0);
  assert(x <= 1);
}