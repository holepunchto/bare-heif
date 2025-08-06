const test = require('brittle')
const heif = require('.')

test('decode .heic', (t) => {
  const image = require('./test/fixtures/grapefruit.heic', {
    with: { type: 'binary' }
  })

  t.comment(heif.decode(image))
})

test('decode .heic, odd width', (t) => {
  const image = require('./test/fixtures/grapefruit-odd-width.heic', {
    with: { type: 'binary' }
  })

  const { width, height, data } = heif.decode(image)

  t.comment({ width, height })
  t.is(width * height * 4, data.byteLength)
})

test('decode .heic, odd height', (t) => {
  const image = require('./test/fixtures/grapefruit-odd-height.heic', {
    with: { type: 'binary' }
  })

  const { width, height, data } = heif.decode(image)

  t.comment({ width, height })
  t.is(width * height * 4, data.byteLength)
})

test('decode .avif', (t) => {
  const image = require('./test/fixtures/grapefruit.avif', {
    with: { type: 'binary' }
  })

  t.comment(heif.decode(image))
})
