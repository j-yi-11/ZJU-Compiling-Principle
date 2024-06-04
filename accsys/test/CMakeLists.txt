set(ACCSYS_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR})
macro(accsys_add_test TESTNAME FILES)
    add_executable(${TESTNAME} ${FILES})
    message(STATUS "Adding accsys test ${TESTNAME}")
    target_link_libraries(${TESTNAME} PRIVATE GTest::gtest_main)
    set_target_properties(${TESTNAME}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${ACCSYS_TEST_DIR}"
            COMMAND ${TESTNAME}
            )

    # Add the test under CTest.
    add_test(${TESTNAME} ${ACCSYS_TEST_DIR}/${TESTNAME} --gtest_color=yes
            --gtest_output=xml:${ACCSYS_TEST_DIR}/${TESTNAME}.xml)

endmacro()


add_subdirectory(ir)