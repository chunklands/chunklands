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
  dts: `${__dirname}/src/${classNameBase}.d.ts`,
  moduleExportsInl: `${__dirname}/src/module_exports.inl`,
  moduleIncludesInl: `${__dirname}/src/module_includes.inl`,
  moduleDts: `${__dirname}/src/module.d.ts`
};

if (fs.existsSync(filePaths.js) || fs.existsSync(filePaths.h) || fs.existsSync(filePaths.cc) || fs.existsSync(filePaths.dts)) {
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

#include "js.h"

namespace chunklands {
  class ${classNameBase} : public JSWrap<${classNameBase}> {
    JS_DECL_WRAP(${classNameBase})

  };
}

#endif
`);
console.info(`  ✓ created '${filePaths.h}'`);

fs.writeFileSync(filePaths.cc, `#include "${classNameBase}.h"

namespace chunklands {
  JS_DEF_WRAP(${classNameBase}, ONE_ARG({
    
  }))
}
`);
console.info(`  ✓ created '${filePaths.cc}'`);

fs.writeFileSync(filePaths.dts, `export {${classNameBase}};

declare class ${classNameBase} {
  
}
`);
console.info(`  ✓ created '${filePaths.dts}'`);

fs.appendFileSync(filePaths.moduleExportsInl, `\nXX(${classNameBase})`);
console.info(`  ✓ added export to '${filePaths.moduleExportsInl}'`);

fs.appendFileSync(filePaths.moduleIncludesInl, `\n#include "${classNameBase}.h"`);
console.info(`  ✓ added include to '${filePaths.moduleIncludesInl}'`);

fs.appendFileSync(filePaths.moduleDts, `export {${classNameBase}} from './${classNameBase}';\n`);
console.info(`  ✓ added include to '${filePaths.moduleDts}'`);
