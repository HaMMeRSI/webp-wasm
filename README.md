# webp.wasm (fork)

> Fork of [nieyuyao/webp-wasm](https://github.com/nieyuyao/webp-wasm) with the full `WebPConfig` and `WebPAnimEncoderOptions` exposed to JavaScript.

WebAssembly port of [libwebp](https://chromium.googlesource.com/webm/libwebp/). Supports encoding and decoding static and animated WebP images.

## What this fork adds

The upstream library only exposes `quality` and `lossless` for animation encoding. This fork passes the **complete libwebp configuration** through to the WASM layer:

| Category | Options |
|---|---|
| **Encoder** | `method`, `pass`, `target_size`, `sns_strength`, `preprocessing` |
| **Filtering** | `filter_strength`, `filter_sharpness`, `filter_type`, `autofilter` |
| **Alpha** | `alpha_quality`, `alpha_compression`, `alpha_filtering` |
| **Animation** | `loop_count`, `kmin`, `kmax`, `minimize_size`, `allow_mixed` |
| **Advanced** | `segments`, `partitions`, `partition_limit`, `use_sharp_yuv`, `near_lossless`, `exact`, `emulate_jpeg_size`, `qmin`, `qmax` |

All options default to `-1` (use libwebp default), so the API is fully backward compatible.

## Install

```shell
npm i github:HaMMeRSI/webp-wasm
```

## Usage

### Basic animation encoding (unchanged from upstream)

```typescript
import { encodeAnimation } from 'wasm-webp'

const webp = await encodeAnimation(width, height, true, frames)
```

### With full encoder options

```typescript
const webp = await encodeAnimation(width, height, true, frames, {
  method: 6,          // compression effort (0-6, higher = slower + smaller)
  pass: 2,            // entropy analysis passes (1-10)
  loop_count: 0,      // 0 = infinite loop
  kmax: 10,           // keyframe interval
  sns_strength: 80,   // spatial noise shaping (0-100)
  filter_strength: 40,
  alpha_quality: 90,
  use_sharp_yuv: 1,   // sharper color conversion
  near_lossless: 60,  // near-lossless preprocessing (0-100, 100 = off)
})
```

### Per-frame quality

```typescript
const frames = [
  { data: rgba1, duration: 100, config: { lossless: 0, quality: 80 } },
  { data: rgba2, duration: 100, config: { lossless: 1 } },
]
const webp = await encodeAnimation(100, 100, true, frames)
```

## Full API

### Encode

- `encoderVersion(): Promise<string>`
- `encodeRGB(rgb, width, height, quality?): Promise<Uint8Array | null>`
- `encodeRGBA(rgba, width, height, quality?): Promise<Uint8Array | null>`
- `encode(data, width, height, hasAlpha, config): Promise<Uint8Array | null>`
- `encodeAnimation(width, height, hasAlpha, frames, options?): Promise<Uint8Array | null>`

### Decode

- `decoderVersion(): Promise<string>`
- `decodeRGB(data): Promise<WebPDecodedImageData | null>`
- `decodeRGBA(data): Promise<WebPDecodedImageData | null>`
- `decodeAnimation(data, hasAlpha): Promise<DecodedWebPAnimationFrame[] | null>`

## WebPAnimEncoderOptions

```typescript
interface WebPAnimEncoderOptions {
  kmin: number              // min keyframe distance
  kmax: number              // max keyframe distance (0 = auto)
  minimize_size: number     // minimize output size (0/1)
  allow_mixed: number       // allow mixed lossy/lossless frames (0/1)
  loop_count: number        // animation loop count (0 = infinite)
  method: number            // compression method (0-6)
  target_size: number       // target file size in bytes (0 = off)
  pass: number              // entropy analysis passes (1-10)
  preprocessing: number     // 0=none, 1=smooth, 2=dither
  sns_strength: number      // spatial noise shaping (0-100)
  filter_strength: number   // deblocking filter strength (0-100)
  filter_sharpness: number  // filter sharpness (0-7)
  filter_type: number       // 0=simple, 1=strong
  autofilter: number        // auto-adjust filter strength (0/1)
  alpha_quality: number     // alpha channel quality (0-100)
  alpha_compression: number // alpha compression (0/1)
  alpha_filtering: number   // 0=none, 1=fast, 2=best
  segments: number          // quality segments (1-4)
  partitions: number        // token partitions (0-3)
  partition_limit: number   // quality degradation limit (0-100)
  use_sharp_yuv: number     // sharper YUV conversion (0/1)
  near_lossless: number     // near-lossless preprocessing (0-100)
  exact: number             // preserve transparent RGB (0/1)
  emulate_jpeg_size: number // JPEG-like file sizing (0/1)
  qmin: number              // min quality bound (0-100)
  qmax: number              // max quality bound (0-100)
}
```

## Building from source

Requires [Emscripten](https://emscripten.org/). See upstream repo for build setup.

```shell
npm run build:esm    # ESM build
npm run build:cjs    # CJS build
```

## Credits

- [libwebp](https://chromium.googlesource.com/webm/libwebp/) by Google
- [nieyuyao/webp-wasm](https://github.com/nieyuyao/webp-wasm) — original WebAssembly bindings
