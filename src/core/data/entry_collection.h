#ifndef _DEADLOCK_CORE_DATA_ENTRY_COLLECTION_H_
#define _DEADLOCK_CORE_DATA_ENTRY_COLLECTION_H_

#include <vector>

#include "entry.h"

#include "../win32_export.h"

namespace deadlock
{
	namespace core
	{
		namespace data
		{
			/// A container that stores all entries.
			/// It also contains an acceleration structure, used to do fast searches,
			/// and it handles finding possible matches for a given search string.
			class _export entry_collection
			{
			protected:

				/// The list of entries
				std::vector<entry> entries;

			public:

			};
		}
	}
}

#endif