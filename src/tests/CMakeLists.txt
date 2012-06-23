file(GLOB DEADLOCK_TEST_FILES *.h *.cpp)
source_group("tests" FILES ${DEADLOCK_TEST_FILES})

file(GLOB_RECURSE DEADLOCK_TEST_HEADERS *.h)
file(GLOB_RECURSE DEADLOCK_TEST_SOURCES *.cpp)

add_executable(tests ${DEADLOCK_TEST_HEADERS} ${DEADLOCK_TEST_SOURCES})
target_link_libraries(tests libdeadlock)
