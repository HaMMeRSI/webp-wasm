#include "emscripten/emscripten.h"
#include "emscripten/bind.h"
#include "encode.h"
#include "decode.h"


EMSCRIPTEN_BINDINGS(module)
{
	emscripten::value_object<SimpleWebPConfig>("SimpleWebPConfig")
		.field("lossless", &SimpleWebPConfig::lossless)
		.field("quality", &SimpleWebPConfig::quality);

	emscripten::value_object<WebPAnimationFrame>("WebPAnimationFrame")
		.field("duration", &WebPAnimationFrame::duration)
		.field("data", &WebPAnimationFrame::data)
		.field("config", &WebPAnimationFrame::config)
		.field("has_config", &WebPAnimationFrame::has_config);

	emscripten::value_object<AnimEncoderOptions>("AnimEncoderOptions")
		.field("kmin", &AnimEncoderOptions::kmin)
		.field("kmax", &AnimEncoderOptions::kmax)
		.field("minimize_size", &AnimEncoderOptions::minimize_size)
		.field("allow_mixed", &AnimEncoderOptions::allow_mixed)
		.field("loop_count", &AnimEncoderOptions::loop_count)
		.field("method", &AnimEncoderOptions::method)
		.field("target_size", &AnimEncoderOptions::target_size)
		.field("pass", &AnimEncoderOptions::pass)
		.field("preprocessing", &AnimEncoderOptions::preprocessing)
		.field("sns_strength", &AnimEncoderOptions::sns_strength)
		.field("filter_strength", &AnimEncoderOptions::filter_strength)
		.field("filter_sharpness", &AnimEncoderOptions::filter_sharpness)
		.field("filter_type", &AnimEncoderOptions::filter_type)
		.field("autofilter", &AnimEncoderOptions::autofilter)
		.field("alpha_quality", &AnimEncoderOptions::alpha_quality)
		.field("alpha_compression", &AnimEncoderOptions::alpha_compression)
		.field("alpha_filtering", &AnimEncoderOptions::alpha_filtering)
		.field("segments", &AnimEncoderOptions::segments)
		.field("partitions", &AnimEncoderOptions::partitions)
		.field("partition_limit", &AnimEncoderOptions::partition_limit)
		.field("use_sharp_yuv", &AnimEncoderOptions::use_sharp_yuv)
		.field("near_lossless", &AnimEncoderOptions::near_lossless)
		.field("exact", &AnimEncoderOptions::exact)
		.field("emulate_jpeg_size", &AnimEncoderOptions::emulate_jpeg_size)
		.field("qmin", &AnimEncoderOptions::qmin)
		.field("qmax", &AnimEncoderOptions::qmax);

	emscripten::register_vector<WebPAnimationFrame>("VectorWebPAnimationFrame");

	emscripten::class_<StreamingAnimEncoder>("StreamingAnimEncoder")
		.constructor<int, int, bool, AnimEncoderOptions>()
		.function("addFrame", &StreamingAnimEncoder::addFrame)
		.function("finalize", &StreamingAnimEncoder::finalize);

	function("encoder_version", &encoder_version);
	function("encodeRGB", &encodeRGB);
	function("encodeRGBA", &encodeRGBA);
	function("encode", &encode);
	function("encodeAnimation", &encodeAnimation);

	function("decoder_version", &decoder_version);
	function("decodeRGB", &decodeRGB);
	function("decodeRGBA", &decodeRGBA);
	function("decode", &decode);
  function("decodeAnimation", &decodeAnimation);
}
