set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB DEADLOCK_CORE_FILES *.h *.cpp)
source_group("core" FILES ${DEADLOCK_CORE_FILES})

file(GLOB DEADLOCK_CORE_CRYPTOGRAPHY_FILES cryptography/*.h cryptography/*.cpp)
source_group("core\\cryptography" FILES ${DEADLOCK_CORE_CRYPTOGRAPHY_FILES})

file(GLOB DEADLOCK_CORE_DATA_FILES data/*.h data/*.cpp)
source_group("core\\data" FILES ${DEADLOCK_CORE_DATA_FILES})

file(GLOB DEADLOCK_CORE_SERIALISATION_FILES serialisation/*.h serialisation/*.cpp)
source_group("core\\serialisation" FILES ${DEADLOCK_CORE_SERIALISATION_FILES})

file(GLOB_RECURSE DEADLOCK_CORE_HEADERS *.h)
file(GLOB_RECURSE DEADLOCK_CORE_SOURCES *.cpp)

add_library(deadlock SHARED ${DEADLOCK_CORE_HEADERS} ${DEADLOCK_CORE_SOURCES})