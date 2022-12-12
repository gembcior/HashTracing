add_library(tracing INTERFACE)

target_include_directories(tracing
  INTERFACE
    inc
)

add_subdirectory(src)

include(Testing)

add_subdirectory(test)
