// Overwrite bytes inside a named ISOBMFF box, keeping the surrounding boxes
// intact. Used to reach the decoder's failure paths without having to ship a
// separate broken fixture for each one.
exports.corruptBox = function corruptBox(image, box, { offset, length }) {
  const buffer = Buffer.from(image)

  const at = buffer.indexOf(Buffer.from(box, 'ascii'))

  if (at === -1) throw new Error(`No '${box}' box in the fixture`)

  buffer.fill(0xff, at + offset, at + offset + length)

  return buffer
}
