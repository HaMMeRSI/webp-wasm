#include <vector>
#include "src/webp/encode.h"
#include "src/webp/mux.h"
#include "emscripten/emscripten.h"
#include "emscripten/val.h"

struct SimpleWebPConfig
{
	int lossless;
  float quality;
};

struct WebPAnimationFrame
{
	int duration;
	std::string data;
	SimpleWebPConfig config;
	bool has_config;
};

struct AnimEncoderOptions
{
	int kmin;
	int kmax;
	int minimize_size;
	int allow_mixed;
	int loop_count;
	int method;
	// Per-frame WebPConfig fields (-1 = use libwebp default)
	int target_size;
	int pass;
	int preprocessing;
	int sns_strength;
	int filter_strength;
	int filter_sharpness;
	int filter_type;
	int autofilter;
	int alpha_quality;
	int alpha_compression;
	int alpha_filtering;
	int segments;
	int partitions;
	int partition_limit;
	int use_sharp_yuv;
	int near_lossless;
	int exact;
	int emulate_jpeg_size;
	int qmin;
	int qmax;
};

emscripten::val encoder_version();

emscripten::val encodeRGB(std::string rgb, int width, int height, int quality_factor);

emscripten::val encodeRGBA(std::string rgba, int width, int height, int quality_factor);

emscripten::val encode(std::string data, int width, int height, bool use_alpha, SimpleWebPConfig config);

emscripten::val encodeAnimation(int width, int height, bool has_alpha, std::vector<WebPAnimationFrame> frames, AnimEncoderOptions options);