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
  const metadata = binding.getMetadata(image, opts.type)

  for (const block of metadata) {
    block.data = Buffer.from(block.data)
  }

  return metadata
}
