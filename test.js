const test = require('brittle')
const heif = require('.')

const heic = require('./test/fixtures/grapefruit.heic', {
  with: { type: 'binary' }
})

test('decode .heic', (t) => {
  t.comment(heif.decode(heic))
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

test('rejects a .jpg', (t) => {
  const image = require('./test/fixtures/grapefruit.jpg', {
    with: { type: 'binary' }
  })

  t.exception(() => heif.decode(image), /No 'ftyp' box/)
})

test('rejects an empty buffer', (t) => {
  t.exception(() => heif.decode(Buffer.alloc(0)), /File size too small/)
})

test('rejects input that is not a HEIF file', (t) => {
  t.exception(
    () => heif.decode(Buffer.from('this is not a heif file at all')),
    /File size too small/
  )
})

test('rejects a truncated file', (t) => {
  t.exception(() => heif.decode(heic.subarray(0, 100)), /Cannot read full meta box/)
})

test('rejects a container with no primary image', (t) => {
  // The 'pitm' box names the primary image. Blanking the item ID it points at
  // leaves a readable container with no image to hand back.
  const image = corruptBox(heic, 'pitm', { offset: 8, length: 2 })

  t.exception(() => heif.decode(image), /non-existing image/)
})

test('rejects an image the decoder cannot read', (t) => {
  // 'mdat' holds the coded image data. Corrupting its head gets past the
  // container parser and fails inside the decoder instead.
  const image = corruptBox(heic, 'mdat', { offset: 8, length: 256 })

  t.exception(() => heif.decode(image), /Decoder plugin generated an error/)
})

// Helpers

// Overwrite bytes inside a named ISOBMFF box, keeping the surrounding boxes
// intact. Used to reach the decoder's failure paths without having to ship a
// separate broken fixture for each one.
function corruptBox(image, box, { offset, length }) {
  const buffer = Buffer.from(image)

  const at = buffer.indexOf(Buffer.from(box, 'ascii'))

  if (at === -1) throw new Error(`No '${box}' box in the fixture`)

  buffer.fill(0xff, at + offset, at + offset + length)

  return buffer
}
