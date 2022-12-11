#ifndef LIB_TRACING_PRINTER_H
#define LIB_TRACING_PRINTER_H

#include "tracing/hashing.h"

#include <array>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace tracing {

class Printer
{
public:
  using OutputFunction = void (*)(const char);

public:
  constexpr Printer()
    : m_out(nullptr)
  {
  }

  void registerOutput(OutputFunction out);
  void printEndLine(bool endLine);

  template<typename... Args>
  void print(const char* text, Args... arguments);

  template<typename T, size_t size, typename... Args>
  void print(const std::array<T, size> text, Args... arguments);

protected:
  enum Color : char
  {
    Black = '0',
    Red = '1',
    Green = '2',
    Yellow = '3',
    Blue = '4',
    Magenta = '5',
    Cyan = '6',
    White = '7',
    Default = '9',
  };

  enum ColorType : char
  {
    Foreground = '3',
    Background = '4',
  };

protected:
  void printColorMark(char mark, char type = ColorType::Foreground);
  void printAttributeMark(char mark);
  void printEndLine();
  void putChar(const char& character)
  {
    if (m_out) {
      m_out(character);
    }
  }

protected:
  OutputFunction m_out;

private:
  enum FormatType : uint32_t
  {
    Bin = 2,
    Oct = 8,
    Dec = 10,
    Hex = 16,
  };

  enum Align
  {
    Start,
    End,
  };

  static constexpr char EscCharacter = 0x1B;
  static constexpr unsigned int MaxDigits = 32;
  static constexpr char ArgumentStartMark = '{';
  static constexpr char ArgumentEndMark = '}';
  static constexpr char ArgumentFormatMark = ':';
  static constexpr char ColorStartMark = '[';
  static constexpr char ColorEndMark = ']';
  static constexpr char ColorNumberMark = ArgumentFormatMark;

private:
  struct ArgumentFormat
  {
    FormatType type = FormatType::Dec;
    Align align = Align::Start;
    uint32_t width = 0;
    bool padding = false;
    bool alternateFormat = false;
  };

private:
  template<typename Arg, typename... Args>
  void mainPrint(const char* text, Arg argument, Args... arguments);
  template<typename Arg, typename... Args>
  void mainPrint(Arg argument, Args... arguments);
  void mainPrint(const char* text);

  bool parseColorMark(const char*& text);
  template<typename Arg>
  bool parseArgumentMark(const char*& text, Arg argument);
  void updateFormat(char text, ArgumentFormat& format);
  void printBuffer(const char* buffer);

  template<typename Arg>
  void printArgument(Arg argument, ArgumentFormat format, std::enable_if_t<std::is_same_v<Arg, bool>>* = nullptr);
  template<typename Arg>
  void printArgument(Arg argument,
                     ArgumentFormat format,
                     std::enable_if_t<std::is_integral_v<Arg> && !std::is_same_v<Arg, bool>>* = nullptr);
  template<typename Arg>
  void printArgument(Arg argument,
                     ArgumentFormat format,
                     std::enable_if_t<std::is_same_v<Arg, char*> || std::is_same_v<Arg, const char*>>* = nullptr);
};

template<typename... Args>
void Printer::print(const char* text, Args... arguments)
{
  mainPrint(text, arguments...);
}

template<typename T, size_t size, typename... Args>
void Printer::print(const std::array<T, size> text, Args... arguments)
{
  if (text.back() != 0) {
    mainPrint("Bad string. Missing Null terminator.");
    return;
  }
  const char* data = reinterpret_cast<const char*>(text.data());
  mainPrint(data, arguments...);
}

template<typename Arg, typename... Args>
void Printer::mainPrint(const char* text, Arg argument, Args... arguments)
{
  while (*text) {
    if (parseArgumentMark(text, argument)) {
      mainPrint(text, arguments...);
      return;
    } else if (parseColorMark(text)) {
      mainPrint(text, argument, arguments...);
      return;
    } else {
      putChar(*text);
      text++;
    }
  }
  mainPrint(argument, arguments...);
}

template<typename Arg, typename... Args>
void Printer::mainPrint(Arg argument, Args... arguments)
{
  constexpr ArgumentFormat format = {
    .type = FormatType::Hex,
  };
  putChar(' ');
  printArgument(argument, format);
  if constexpr (sizeof...(Args) > 0) {
    mainPrint(arguments...);
    return;
  } else {
    printEndLine();
  }
}

template<typename Arg>
bool Printer::parseArgumentMark(const char*& text, Arg argument)
{
  if (*text == ArgumentStartMark) {
    text++;
    ArgumentFormat format;
    if (*text == ArgumentFormatMark) {
      text++;
      while (*text != ArgumentEndMark) {
        updateFormat(*text, format);
        text++;
      }
      text++;
      printArgument(argument, format);
      return true;
    } else if (*text == ArgumentEndMark) {
      text++;
      printArgument(argument, format);
      return true;
    } else {
      text--;
    }
  }
  return false;
}

template<typename Arg>
void Printer::printArgument(Arg argument, ArgumentFormat format, std::enable_if_t<std::is_same_v<Arg, bool>>*)
{
  if (argument) {
    if (m_out) {
      m_out('t');
      m_out('r');
      m_out('u');
      m_out('e');
    }
  } else {
    if (m_out) {
      m_out('f');
      m_out('a');
      m_out('l');
      m_out('s');
      m_out('e');
    }
  }
}

template<typename Arg>
void Printer::printArgument(Arg argument, ArgumentFormat format, std::enable_if_t<std::is_integral_v<Arg> && !std::is_same_v<Arg, bool>>*)
{
  char buffer[MaxDigits] = {};
  bool lessThanZero = false;

  uint32_t base = static_cast<uint32_t>(format.type);

  if (argument < 0) {
    argument *= -1;
    lessThanZero = true;
  }

  std::to_chars(buffer, buffer + MaxDigits, argument, base);
  uint32_t size = std::strlen(buffer);
  if (lessThanZero) {
    size++;
  }

  auto printAlign = [&](char character) {
    if (format.width > size) {
      for (uint32_t i = 0; i < (format.width - size); i++) {
        if (m_out)
          m_out(character);
      }
    }
  };

  if ((format.align == Align::Start) && !format.padding) {
    printAlign(' ');
  }

  if (lessThanZero) {
    putChar('-');
  }

  if (format.alternateFormat) {
    switch (format.type) {
      case FormatType::Hex:
        size += 2;
        if (m_out) {
          m_out('0');
          m_out('x');
        }
        break;
      case FormatType::Bin:
        size += 2;
        if (m_out) {
          m_out('0');
          m_out('b');
        }
        break;
      case FormatType::Oct:
        size += 1;
        putChar('0');
      default:
        break;
    }
  }

  if (format.padding && format.align != Align::End) {
    printAlign('0');
  }

  printBuffer(buffer);

  if (format.align == Align::End) {
    printAlign(' ');
  }
}

template<typename Arg>
void Printer::printArgument(Arg argument,
                            ArgumentFormat format,
                            std::enable_if_t<std::is_same_v<Arg, char*> || std::is_same_v<Arg, const char*>>*)
{
  uint32_t size = std::strlen(argument);

  auto printAlign = [&](char character) {
    if (format.width > size) {
      for (uint32_t i = 0; i < (format.width - size); i++) {
        putChar(character);
      }
    }
  };

  if (format.align == Align::Start) {
    printAlign(' ');
  }

  printBuffer(argument);

  if (format.align == Align::End) {
    printAlign(' ');
  }
}

}

#endif /* LIB_TRACING_PRINTER_H */
