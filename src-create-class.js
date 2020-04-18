const fs = require('fs');

const className = process.argv[2];
if (!className) {
  console.error(`  ✘ classname missing`);
  process.exit(1);
  return;
}

const filePaths = {
  h: `${__dirname}/src/${className}.h`,
  cc: `${__dirname}/src/${className}.cc`
};

if (fs.existsSync(filePaths.h) || fs.existsSync(filePaths.cc)) {
  console.error('  ✘ unsafe generating code, one of the files exist.');
  process.exit(1);
  return;
}

fs.writeFileSync(filePaths.h, `#ifndef __CHUNKLANDS_${className.toUpperCase()}_H__
#define __CHUNKLANDS_${className.toUpperCase()}_H__

namespace chunklands {
  class ${className} {

  };
}

#endif
`);
console.info(`  ✓ created '${filePaths.h}'`);

fs.writeFileSync(filePaths.cc, `#include "${className}.h"

namespace chunklands {

}
`);
console.info(`  ✓ created '${filePaths.cc}'`);
