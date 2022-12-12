include(Testing)

testing_target_add_test(tracing
  PrinterOutputBuffer.cpp
  PrinterTest.cpp
)

testing_target_test_link_libraries(tracing
  fmt
)

testing_target_test_link_mock_libraries(tracing
  tracing
)
