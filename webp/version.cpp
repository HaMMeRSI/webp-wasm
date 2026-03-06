#include <string.h>
#include "version.h"

using namespace emscripten;

val get_version(int hex_version)
{
	int rev_ver = hex_version & 0xff;
	int min_ver = (hex_version >> 8) & 0xff;
	int maj_ver = (hex_version >> 16) & 0xff;
	int vers[3] = {
		maj_ver,
		min_ver,
		rev_ver,
	};
	std::string ver = "";

	for (int i : vers)
	{
		ver.append(std::to_string(i)).append(".");
	}
	ver.pop_back();
	return val::u8string(ver.c_str());
}