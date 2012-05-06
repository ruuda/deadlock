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

bool operator<(version v1, version v2)
{
	if (v1.major < v2.major) return true; if (v1.major > v2.major) return false;
	if (v1.minor < v2.minor) return true; if (v1.minor > v2.minor) return false;
	if (v1.revision < v2.revision) return true; if (v1.revision > v2.revision) return false;
	if (v1.build < v2.build) return true; if (v1.build > v2.build) return true;

	// Equality is not less-than
	return false;
}

bool operator<=(version v1, version v2)
{
	if (v1.major < v2.major) return true; if (v1.major > v2.major) return false;
	if (v1.minor < v2.minor) return true; if (v1.minor > v2.minor) return false;
	if (v1.revision < v2.revision) return true; if (v1.revision > v2.revision) return false;
	if (v1.build < v2.build) return true; if (v1.build > v2.build) return true;

	// Equality is less-than or equal
	return true;
}

std::ostream& operator<<(std::ostream& ostr, const version& v)
{
	ostr << v.major << "." << v.minor << "." << v.revision << "." << v.build;
	return ostr;
}