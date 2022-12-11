#include "tracing/printer.h"

namespace tracing {

void Printer::registerOutput(OutputFunction out)
{
  m_out = out;
}

void Printer::printEndLine()
{
  putChar('\n');
}

void Printer::printColorMark(char mark, char type)
{
  if (m_out) {
    m_out(EscCharacter);
    m_out('[');
    m_out(type);
    m_out(mark);
    m_out('m');
  }
}

void Printer::printAttributeMark(char mark)
{
  if (m_out) {
    m_out(EscCharacter);
    m_out('[');
    m_out(mark);
    m_out('m');
  }
}

bool Printer::parseColorMark(const char*& text)
{
  if (*text == ColorStartMark) {
    text++;
    if (*text == ColorNumberMark) {
      text++;
      char colorMark = Color::Default;
      char colorType = ColorType::Foreground;
      while (*text != ColorEndMark) {
        if (*text == 'f') {
          colorType = ColorType::Foreground;
        } else if (*text == 'b') {
          colorType = ColorType::Background;
        } else if (*text >= Black && *text <= Default) {
          colorMark = *text;
        } else {
          colorMark = Color::Default;
          colorType = ColorType::Foreground;
        }
        text++;
      }
      text++;
      printColorMark(colorMark, colorType);
      return true;
    } else if (*text == ColorEndMark) {
      text++;
      printColorMark(Default);
      return true;
    } else {
      text--;
    }
  }
  return false;
}

void Printer::updateFormat(char text, ArgumentFormat& format)
{
  if (text == '#') {
    format.alternateFormat = true;
  } else if (text == '<') {
    format.align = Align::End;
  } else if (text == '>') {
    format.align = Align::Start;
  } else if (text == 'x') {
    format.type = FormatType::Hex;
  } else if (text == 'b') {
    format.type = FormatType::Bin;
  } else if (text == 'd') {
    format.type = FormatType::Dec;
  } else if (text == 'o') {
    format.type = FormatType::Oct;
  } else if (text >= '0' && text <= '9') {
    if (text == '0' && format.width == 0) {
      format.padding = true;
    } else {
      format.width = (format.width * 10) + (text - '0');
    }
  }
}

void Printer::mainPrint(const char* text)
{
  while (*text) {
    if (parseColorMark(text)) {
      mainPrint(text);
      return;
    } else {
      putChar(*text);
      text++;
    }
  }
  printEndLine();
}

void Printer::printBuffer(const char* buffer)
{
  while (*buffer) {
    putChar(*buffer);
    buffer++;
  }
}

}
