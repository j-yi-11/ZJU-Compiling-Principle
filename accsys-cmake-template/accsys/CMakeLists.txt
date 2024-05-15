
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ACCSYS_MAIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ACCSYS_MAIN_INCLUDE_DIR ${ACCSYS_MAIN_SRC_DIR}/include)
set(ACCSYS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${ACCSYS_MAIN_INCLUDE_DIR})



add_subdirectory(lib)

# accsys tests
if (ACCSYS_BUILD_TESTS)
    # build gtest
    add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/googletest "third_party/googletest")
    set(GTEST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/googletest/include)
    add_subdirectory(test)
endif ()