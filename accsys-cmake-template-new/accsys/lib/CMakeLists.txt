add_subdirectory(ir)

# define the accsys library.
add_library(accsys INTERFACE)
# link accipit ir library component.
target_link_libraries(accsys INTERFACE accsys::ir)
add_library(accsys::accsys ALIAS accsys)

target_include_directories(accsys INTERFACE ${ACCSYS_MAIN_INCLUDE_DIR})