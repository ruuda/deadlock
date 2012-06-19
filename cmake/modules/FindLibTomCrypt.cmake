# - Try to find LibTomCrypt
# Once done this will define
# LibTomCrypt_FOUND - System has LibTomCrypt
# LibTomCrypt_INCLUDE_DIRS - The LibTomCrypt include directories
# LibTomCrypt_LIBRARIES - The libraries needed to use LibTomCrypt

find_path(LibTomCrypt_INCLUDE_DIR "src/headers/tomcrypt.h")
find_library(LibTomCrypt_LIBRARY tomcrypt)

set(LibTomCrypt_LIBRARIES ${LibTomCrypt_LIBRARY})
set(LibTomCrypt_INCLUDE_DIRS ${LibTomCrypt_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibTomCrypt DEFAULT_MSG LibTomCrypt_LIBRARY LibTomCrypt_INCLUDE_DIR)

mark_as_advanced(LibTomCrypt_INCLUDE_DIR LibTomCrypt_LIBRARY)