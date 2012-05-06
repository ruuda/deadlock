#ifndef _DEADLOCK_CORE_H_
#define _DEADLOCK_CORE_H_

#include "document.h"

#include "win32_export.h"

namespace deadlock
{
	namespace core
	{
		/// Provides information about Deadlock
		class _export assembly_information
		{
		public:

			/// Returns the Deadlock core (libdeadlock) version
			static version get_version();
		};
	}
}

#endif