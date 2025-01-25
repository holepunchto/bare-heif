# bare-heif

HEIF support for Bare.

```
npm i bare-heif
```

## Usage

```js
const heif = require('bare-hare')

const image = require('./my-image.heic', { with: { type: 'binary' } })

heif.decode(image)
// {
//   width: 200,
//   height: 400,
//   data: <Buffer>
// }
```

## License

Apache-2.0
