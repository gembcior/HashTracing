include(Utils)

add_executable(hashing
  main.cpp
)

target_include_directories(hashing
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hashing
  PRIVATE
    tracing
)

target_app_release(hashing)
