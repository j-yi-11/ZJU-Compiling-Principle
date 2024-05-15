add_library(accipit SHARED
    ir.cpp
    type.cpp
    ir_writer.cpp
)
set_target_properties(accipit PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
add_library(accsys::ir ALIAS accipit)