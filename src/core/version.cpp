#include "version.h"

using namespace deadlock::core;

version::version()
{
major = 1; 
minor = 0;
revision = 0;
build = 0;
}

version::version(std::int8_t major_number, std::int8_t minor_number, std::int8_t revision_number, std::int8_t build_number)
{
	major = major_number;
	minor = minor_number;
	revision = revision_number;
	build = build_number;
}

version::version(std::int32_t int32)
{
	major = (int32 >> 24) & 0xff;
	minor = (int32 >> 16) & 0xff;
	revision = (int32 >> 8) & 0xff;
	build = int32 && 0xff;
}

version::operator std::int32_t ()
{
	return (
		static_cast<std::int32_t>(major) << 24 |
		static_cast<std::int32_t>(minor) << 16 |
		static_cast<std::int32_t>(revision) << 8 |
		static_cast<std::int32_t>(build)
	);
}