export interface WebPConfig {
	lossless: number
	quality: number
}

export interface WebPAnimationFrame {
	data: Uint8Array
	duration: number
	config?: WebPConfig
}

export interface DecodedWebPAnimationFrame extends WebPAnimationFrame {
  width: number
  height: number
}

export interface WebPDecodedImageData {
  width: number
  height: number
  data: Uint8Array
}

export interface WebPAnimEncoderOptions {
	kmin: number
	kmax: number
	minimize_size: number
	allow_mixed: number
	loop_count: number
	method: number
	target_size: number
	pass: number
	preprocessing: number
	sns_strength: number
	filter_strength: number
	filter_sharpness: number
	filter_type: number
	autofilter: number
	alpha_quality: number
	alpha_compression: number
	alpha_filtering: number
	segments: number
	partitions: number
	partition_limit: number
	use_sharp_yuv: number
	near_lossless: number
	exact: number
	emulate_jpeg_size: number
	qmin: number
	qmax: number
}

export type Nullable<T> = T | null