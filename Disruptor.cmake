add_library(Disruptor STATIC IMPORTED)
set_target_properties(Disruptor PROPERTIES IMPORTED_LOCATION ${REPO_BUILT_LIB_DIR}/libdisruptor.a)

include_directories(${CMAKE_CURRENT_LIST_DIR})

set(DISRUPTOR_LIBRARIES Disruptor)
