set(ACCSYS_LINK_COMPONENTS
    accsys::ir
)

set(ACCSYS_TEST_SOURCES
    type_test.cpp
    instruction_test.cpp
    function_test.cpp
)
accsys_add_test(IRTest
    "${ACCSYS_TEST_SOURCES}"
)
target_link_libraries(IRTest PRIVATE ${ACCSYS_LINK_COMPONENTS})