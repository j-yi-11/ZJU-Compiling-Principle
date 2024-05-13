add_subdirectory(ir)

# define the accsys library.
add_library(accsys INTERFACE)
# set debug configuration library postfix
set_target_properties(accsys PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
# link accipit ir library component.
target_link_libraries(accsys INTERFACE accsys::ir)
add_library(accsys::accsys ALIAS accsys)

target_include_directories(accsys INTERFACE ${ACCSYS_MAIN_INCLUDE_DIR})