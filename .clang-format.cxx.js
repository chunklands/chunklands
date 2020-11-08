const {promisify} = require('util')
const glob = promisify(require('glob'))
const execa = require('execa')

const SRC = `${__dirname}/src`

;(async () => {
  
  const matches = await glob(`${SRC}/**/*{.cxx,.hxx}`)
  await execa('clang-format', [...matches])

})().catch(err => {
  console.error(err)
  process.exit(1)
})

