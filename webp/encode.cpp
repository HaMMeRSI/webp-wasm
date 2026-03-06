#include <string.h>
#include "version.h"
#include "encode.h"

using namespace emscripten;
thread_local val Uint8Array = val::global("Uint8Array");

void applyPicConfig(WebPConfig& webp_config, const SimpleWebPConfig& config)
{
	webp_config.quality = config.quality;
	webp_config.lossless = config.lossless;
}

val encoder_version()
{
	return get_version(WebPGetEncoderVersion());
}

val encodeRGB(std::string rgb, int width, int height, int quality_factor) {
	uint8_t* output;
	size_t size = WebPEncodeRGB((uint8_t*)rgb.c_str(), width, height, 3 * width, quality_factor, &output);
	if (size == 0) return val::null();
	val encoded_data = Uint8Array.new_(typed_memory_view(size, output));
	WebPFree(output);
	return encoded_data;
}

val encodeRGBA(std::string rgba, int width, int height, int quality_factor) {
	uint8_t* output;
	size_t size = WebPEncodeRGBA((uint8_t*)rgba.c_str(), width, height, 4 * width, quality_factor, &output);
	if (size == 0) return val::null();
	val encoded_data = Uint8Array.new_(typed_memory_view(size, output));
	WebPFree(output);
	return encoded_data;
}

val encode(std::string data, int width, int height, bool has_alpha, SimpleWebPConfig config)
{
	WebPConfig webp_config;
	WebPConfigInit(&webp_config);
	applyPicConfig(webp_config, config);

	WebPMemoryWriter wrt;
	WebPMemoryWriterInit(&wrt);

	WebPPicture pic;
	if (!WebPPictureInit(&pic))
	{
		return val::null();
	}
	// only support argb!!!
	pic.use_argb = 1;
	pic.width = width;
	pic.height = height;
	pic.writer = WebPMemoryWrite;
	pic.custom_ptr = &wrt;
	int import_ok = has_alpha
		? WebPPictureImportRGBA(&pic, (uint8_t*)data.c_str(), width * 4)
		: WebPPictureImportRGB(&pic, (uint8_t*)data.c_str(), width * 3);
	if (!import_ok)
	{
		WebPPictureFree(&pic);
		return val::null();
	}
	int success = WebPEncode(&webp_config, &pic);
	if (!success)
	{
		WebPPictureFree(&pic);
		WebPMemoryWriterClear(&wrt);
		return val::null();
	}
	val encoded_data = Uint8Array.new_(typed_memory_view(wrt.size, wrt.mem));
	WebPPictureFree(&pic);
	WebPMemoryWriterClear(&wrt);
	return encoded_data;
}

val encodeAnimation(int width, int height, bool has_alpha, std::vector<WebPAnimationFrame> frames, AnimEncoderOptions options)
{
	WebPAnimEncoderOptions enc_options;
	WebPAnimEncoderOptionsInit(&enc_options);
	enc_options.kmin = options.kmin;
	enc_options.kmax = options.kmax;
	enc_options.minimize_size = options.minimize_size;
	enc_options.allow_mixed = options.allow_mixed;
	WebPAnimEncoder* enc = WebPAnimEncoderNew(width, height, &enc_options);
	int stride = (has_alpha ? 4 : 3) * width;
	int timestamp = 0;
	for (const auto& frame : frames)
	{
		WebPConfig config;
		WebPConfigInit(&config);
		if (frame.has_config)
		{
			applyPicConfig(config, frame.config);
		}
		if (options.method >= 0) config.method = options.method;
		if (options.target_size >= 0) config.target_size = options.target_size;
		if (options.pass >= 0) config.pass = options.pass;
		if (options.preprocessing >= 0) config.preprocessing = options.preprocessing;
		if (options.sns_strength >= 0) config.sns_strength = options.sns_strength;
		if (options.filter_strength >= 0) config.filter_strength = options.filter_strength;
		if (options.filter_sharpness >= 0) config.filter_sharpness = options.filter_sharpness;
		if (options.filter_type >= 0) config.filter_type = options.filter_type;
		if (options.autofilter >= 0) config.autofilter = options.autofilter;
		if (options.alpha_quality >= 0) config.alpha_quality = options.alpha_quality;
		if (options.alpha_compression >= 0) config.alpha_compression = options.alpha_compression;
		if (options.alpha_filtering >= 0) config.alpha_filtering = options.alpha_filtering;
		if (options.segments >= 0) config.segments = options.segments;
		if (options.partitions >= 0) config.partitions = options.partitions;
		if (options.partition_limit >= 0) config.partition_limit = options.partition_limit;
		if (options.use_sharp_yuv >= 0) config.use_sharp_yuv = options.use_sharp_yuv;
		if (options.near_lossless >= 0) config.near_lossless = options.near_lossless;
		if (options.exact >= 0) config.exact = options.exact;
		if (options.emulate_jpeg_size >= 0) config.emulate_jpeg_size = options.emulate_jpeg_size;
		if (options.qmin >= 0) config.qmin = options.qmin;
		if (options.qmax >= 0) config.qmax = options.qmax;

		WebPPicture pic;
		if (!WebPPictureInit(&pic))
		{
			WebPAnimEncoderDelete(enc);
			return val::null();
		}
		pic.use_argb = 1;
		pic.width = width;
		pic.height = height;
		int import_ok = has_alpha
			? WebPPictureImportRGBA(&pic, (uint8_t*)frame.data.c_str(), stride)
			: WebPPictureImportRGB(&pic, (uint8_t*)frame.data.c_str(), stride);
		if (!import_ok)
		{
			WebPPictureFree(&pic);
			WebPAnimEncoderDelete(enc);
			return val::null();
		}
		int success = WebPAnimEncoderAdd(enc, &pic, timestamp, &config);
		WebPPictureFree(&pic);
		timestamp = timestamp + frame.duration;
		if (!success) {
			WebPAnimEncoderDelete(enc);
			return val::null();
		}
	}

	WebPAnimEncoderAdd(enc, NULL, timestamp, NULL);

	WebPData webp_data;
	WebPDataInit(&webp_data);
	if (!WebPAnimEncoderAssemble(enc, &webp_data)) {
		WebPAnimEncoderDelete(enc);
		return val::null();
	}
	WebPAnimEncoderDelete(enc);

	if (options.loop_count != 0)
	{
		WebPMux* mux = WebPMuxCreate(&webp_data, 1);
		if (mux == NULL)
		{
			WebPDataClear(&webp_data);
			return val::null();
		}
		WebPMuxAnimParams params;
		WebPMuxGetAnimationParams(mux, &params);
		params.loop_count = options.loop_count;
		WebPMuxSetAnimationParams(mux, &params);
		WebPDataClear(&webp_data);
		if (WebPMuxAssemble(mux, &webp_data) != WEBP_MUX_OK)
		{
			WebPMuxDelete(mux);
			return val::null();
		}
		WebPMuxDelete(mux);
	}

	val encoded_data = Uint8Array.new_(typed_memory_view(webp_data.size, webp_data.bytes));
	WebPDataClear(&webp_data);
	return encoded_data;
}