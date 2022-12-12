add_library(tracing INTERFACE)

target_include_directories(tracing
  INTERFACE
    inc
)

add_subdirectory(src)

include(Testing)

testing_target_add_mock_library(tracing)

add_subdirectory(test)
