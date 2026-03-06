import type {
  WebPConfig,
  Nullable,
  WebPAnimationFrame,
  WebPAnimEncoderOptions,
  WebPDecodedImageData,
  DecodedWebPAnimationFrame,
  AnimEncoder,
} from './types'
// @ts-ignore
import Module from './webp-wasm'

// default webp config
const defaultWebpConfig: WebPConfig = {
  lossless: 0,
  quality: 100,
}

export const encoderVersion = async (): Promise<string> => {
  const module = await Module()
  return module.encoder_version()
}

export const encodeRGB = async (
  rgb: Uint8Array,
  width: number,
  height: number,
  quality?: number
): Promise<Nullable<Uint8Array>> => {
  const module = await Module()
  quality = typeof quality !== 'number' ? 100 : Math.min(100, Math.max(0, quality))
  return module.encodeRGB(rgb, width, height, quality)
}

export const encodeRGBA = async (
  rgba: Uint8Array,
  width: number,
  height: number,
  quality?: number
): Promise<Nullable<Uint8Array>> => {
  const module = await Module()
  quality = typeof quality !== 'number' ? 100 : Math.min(100, Math.max(0, quality))
  return module.encodeRGBA(rgba, width, height, quality)
}

export const encode = async (
  data: Uint8Array,
  width: number,
  height: number,
  hasAlpha: boolean,
  config: Partial<WebPConfig>
): Promise<Nullable<Uint8Array>> => {
  const module = await Module()
  const webpConfig = {
    ...defaultWebpConfig,
    ...config,
  }
  webpConfig.lossless = Math.min(1, Math.max(0, webpConfig.lossless))
  webpConfig.quality = Math.min(100, Math.max(0, webpConfig.quality))
  return module.encode(data, width, height, hasAlpha, webpConfig)
}

const defaultAnimEncoderOptions: WebPAnimEncoderOptions = {
  kmin: 0,
  kmax: 0,
  minimize_size: 0,
  allow_mixed: 0,
  loop_count: 0,
  method: -1,
  target_size: -1,
  pass: -1,
  preprocessing: -1,
  sns_strength: -1,
  filter_strength: -1,
  filter_sharpness: -1,
  filter_type: -1,
  autofilter: -1,
  alpha_quality: -1,
  alpha_compression: -1,
  alpha_filtering: -1,
  segments: -1,
  partitions: -1,
  partition_limit: -1,
  use_sharp_yuv: -1,
  near_lossless: -1,
  exact: -1,
  emulate_jpeg_size: -1,
  qmin: -1,
  qmax: -1,
}

export const encodeAnimation = async (
  width: number,
  height: number,
  hasAlpha: boolean,
  frames: WebPAnimationFrame[],
  options?: Partial<WebPAnimEncoderOptions>
): Promise<Nullable<Uint8Array>> => {
  const module = await Module()
  const frameVector = new module.VectorWebPAnimationFrame()
  frames.forEach((frame) => {
    const hasConfig = frame.config !== undefined
    const config = {
      ...defaultWebpConfig,
      ...frame.config,
    }
    config.lossless = Math.min(1, Math.max(0, config.lossless))
    config.quality = Math.min(100, Math.max(0, config.quality))
    frameVector.push_back({
      duration: frame.duration,
      data: frame.data,
      config,
      has_config: hasConfig,
    })
  })
  const opts = { ...defaultAnimEncoderOptions, ...options }
  return module.encodeAnimation(width, height, hasAlpha, frameVector, opts)
}

export const createAnimEncoder = async (
  width: number,
  height: number,
  hasAlpha: boolean,
  options?: Partial<WebPAnimEncoderOptions>,
): Promise<AnimEncoder> => {
  const module = await Module()
  const opts = { ...defaultAnimEncoderOptions, ...options }
  const encoder = new module.StreamingAnimEncoder(width, height, hasAlpha, opts)
  let released = false

  const release = () => {
    if (released) return
    encoder.delete()
    released = true
  }

  return {
    addFrame(data: Uint8Array, duration: number, config?: WebPConfig): boolean {
      if (released) return false
      const hasConfig = config !== undefined
      const frameConfig = { ...defaultWebpConfig, ...config }
      frameConfig.lossless = Math.min(1, Math.max(0, frameConfig.lossless))
      frameConfig.quality = Math.min(100, Math.max(0, frameConfig.quality))
      return encoder.addFrame(data, duration, frameConfig, hasConfig)
    },
    finalize(): Nullable<Uint8Array> {
      if (released) return null
      try {
        return encoder.finalize()
      } finally {
        release()
      }
    },
    dispose(): void {
      release()
    },
  }
}

export const decoderVersion = async (): Promise<string> => {
  const module = await Module()
  return module.decoder_version()
}

export const decodeRGB = async (data: Uint8Array): Promise<Nullable<WebPDecodedImageData>> => {
  const module = await Module()
  return module.decodeRGB(data)
}

export const decodeRGBA = async (data: Uint8Array): Promise<Nullable<WebPDecodedImageData>> => {
  const module = await Module()
  return module.decodeRGBA(data)
}

// TODO:
// export const decode = async (data: Uint8Array, hasAlpha: boolean) => {
// 	const module = await Module()
// 	return module.decode(data, hasAlpha)
// }

export const decodeAnimation = async (
  data: Uint8Array,
  hasAlpha: boolean
): Promise<Nullable<DecodedWebPAnimationFrame[]>> => {
  const module = await Module()
  return module.decodeAnimation(data, hasAlpha)
}
