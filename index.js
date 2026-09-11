const binding = require('./binding')

exports.decode = function decode(image) {
  const { width, height, data } = binding.decode(image)

  return {
    width,
    height,
    data: Buffer.from(data)
  }
}

exports.getMetadata = function getMetadata(image, opts = {}) {
  const EXIF_BLOCK_TYPE = 'Exif'
  const EXIF_OFFSET_FIELD_LENGTH = 4

  const metadata = binding.getMetadata(image, opts.type)

  for (const block of metadata) {
    block.data = Buffer.from(block.data)
    if (block.type === EXIF_BLOCK_TYPE) {
      if (block.data.byteLength < EXIF_OFFSET_FIELD_LENGTH) {
        throw new Error('Exif metadata is too short to contain an offset')
      }
      const offset = EXIF_OFFSET_FIELD_LENGTH + block.data.readUIntBE(0, EXIF_OFFSET_FIELD_LENGTH)
      block.data = block.data.subarray(offset)
    }
  }

  return metadata
}
