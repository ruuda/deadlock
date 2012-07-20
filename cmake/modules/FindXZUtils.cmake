# - Try to find XZ Utils
# Once done this will define
# XZUtils_FOUND - System has XZ Utils
# XZUtils_INCLUDE_DIRS - The XZ Utils  include directories
# XZUtils_LIBRARIES - The libraries needed to use XZ Utils

find_path(XZUtils_INCLUDE_DIR "lzma.h")
find_library(XZUtils_LIBRARY liblzma)

set(XZUtils_LIBRARIES ${XZUtils_LIBRARY})
set(XZUtils_INCLUDE_DIRS ${XZUtils_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XZUtils DEFAULT_MSG XZUtils_LIBRARY XZUtils_INCLUDE_DIR)

mark_as_advanced(XZUtils_INCLUDE_DIR XZUtils_LIBRARY)
