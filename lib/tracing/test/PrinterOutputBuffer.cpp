#include "PrinterOutputBuffer.h"

using namespace std;

string PrinterOutputBuffer::m_buffer;

void PrinterOutputBuffer::outputFunction(const char character)
{
  m_buffer += character;
}

const char* PrinterOutputBuffer::getPointer()
{
  return m_buffer.c_str();
}

void PrinterOutputBuffer::clear()
{
  m_buffer.clear();
}
