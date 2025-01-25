const test = require('brittle')
const heif = require('.')

test('decode .heic', (t) => {
  const image = require('./test/fixtures/grapefruit.heic', {
    with: { type: 'binary' }
  })

  t.comment(heif.decode(image))
})

test('decode .avif', (t) => {
  const image = require('./test/fixtures/grapefruit.avif', {
    with: { type: 'binary' }
  })

  t.comment(heif.decode(image))
})
