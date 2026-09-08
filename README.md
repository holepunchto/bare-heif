# bare-heif

HEIF support for Bare.

```
npm i bare-heif
```

## Usage

```js
const heif = require('bare-heif')

const image = require('./my-image.heic', { with: { type: 'binary' } })

// Decode to RGBA
heif.decode(image)
// {
//   width: 200,
//   height: 400,
//   data: <Buffer>
// }

// Read metadata items
heif.getMetadata(image)
// [
//   {
//     type: 'Exif',
//     data: <Buffer>
//   },
//   ...
// ]

// Filter metadata by its four-character HEIF item type
const [exifMetadata] = heif.getMetadata(image, 'Exif')

// Skip the metadata offset to get the raw EXIF data
const offset = exifMetadata.data.readUInt32BE(0)
const exif = exifMetadata.data.subarray(4 + offset)
```

## License

Apache-2.0
