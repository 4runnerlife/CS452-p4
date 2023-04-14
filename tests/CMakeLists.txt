include(FetchContent)

FetchContent_Declare(
  gtest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG main
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_forct_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test-solution.cpp)
  add_executable(test-lab test-solution.cpp)
else()
  add_executable(test-lab test-lab.cpp)
endif()

target_link_libraries(test-lab PRIVATE gtest_main lab)


## Add address sanatizers for debug build only!
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_sanitizers(test-lab)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

include(CTest)
include(GoogleTest)
gtest_discover_tests(test-lab)
