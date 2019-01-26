const fs = require('fs');

const className = process.argv[2];
if (!className) {
  console.error(`  ✘ classname missing`);
  process.exit(1);
  return;
}

const classNameBase = `${className}Base`;

const filePaths = {
  js: `${__dirname}/src/${className}.js`,
  h: `${__dirname}/src/${classNameBase}.h`,
  cc: `${__dirname}/src/${classNameBase}.cc`,
  moduleExportsInl: `${__dirname}/src/module_exports.inl`,
  moduleIncludesInl: `${__dirname}/src/module_includes.inl`
};

if (fs.existsSync(filePaths.js) || fs.existsSync(filePaths.h) || fs.existsSync(filePaths.cc)) {
  console.error('  ✘ unsafe generating code, one of the files exist.');
  process.exit(1);
  return;
}

fs.writeFileSync(filePaths.js, `const {${classNameBase}} = require('./module');

module.exports = class ${className} extends ${classNameBase} {
};
`);
console.info(`  ✓ created '${filePaths.js}'`);

fs.writeFileSync(filePaths.h, `#ifndef __CHUNKLANDS_${classNameBase.toUpperCase()}_H__
#define __CHUNKLANDS_${classNameBase.toUpperCase()}_H__

#include <napi.h>
#include "napi/object_wrap_util.h"

namespace chunklands {
  class ${classNameBase} : public Napi::ObjectWrap<${classNameBase}> {
    DECLARE_OBJECT_WRAP(${classNameBase})

  };
}

#endif
`);
console.info(`  ✓ created '${filePaths.h}'`);

fs.writeFileSync(filePaths.cc, `#include "${classNameBase}.h"

namespace chunklands {
  DEFINE_OBJECT_WRAP_DEFAULT_CTOR(${classNameBase}, ONE_ARG({
    
  }))
}
`);
console.info(`  ✓ created '${filePaths.cc}'`);

fs.appendFileSync(filePaths.moduleExportsInl, `\nXX(${classNameBase})`);
console.info(`  ✓ added export to '${filePaths.moduleExportsInl}'`);

fs.appendFileSync(filePaths.moduleIncludesInl, `\n#include "${classNameBase}.h"`);
console.info(`  ✓ added include to '${filePaths.moduleIncludesInl}'`);