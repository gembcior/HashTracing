#ifndef TRACING_TEST_PRINTER_OUTPUT_BUFFER_H
#define TRACING_TEST_PRINTER_OUTPUT_BUFFER_H

#include <string>

class PrinterOutputBuffer
{
public:
  static void outputFunction(const char character);
  static const char* getPointer();
  static void clear();

private:
  static std::string m_buffer;
};

#endif /* TRACING_TEST_PRINTER_OUTPUT_BUFFER_H */
