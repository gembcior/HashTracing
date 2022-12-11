#ifndef LIB_TRACING_TRACE_H
#define LIB_TRACING_TRACE_H

#include <array>
#include <cstdint>
#include <cstring>

namespace tracing {

class Trace
{
private:
  static constexpr size_t LevelMarkSize = 2U;

public:
  template<size_t size>
  static constexpr std::array<unsigned char, (size + Trace::LevelMarkSize)> info(const char (&text)[size])
  {
    std::array<unsigned char, size + Trace::LevelMarkSize> info{};
    info[0] = 'I';
    info[1] = ':';

    for (unsigned int i = 0; i < size; i++) {
      info[i + LevelMarkSize] = text[i];
    }

    return info;
  }

  template<size_t size>
  static constexpr std::array<unsigned char, (size + Trace::LevelMarkSize)> warning(const char (&text)[size])
  {
    std::array<unsigned char, size + Trace::LevelMarkSize> info{};
    info[0] = 'W';
    info[1] = ':';

    for (unsigned int i = 0; i < size; i++) {
      info[i + LevelMarkSize] = text[i];
    }

    return info;
  }

  template<size_t size>
  static constexpr std::array<unsigned char, (size + Trace::LevelMarkSize)> error(const char (&text)[size])
  {
    std::array<unsigned char, size + Trace::LevelMarkSize> info{};
    info[0] = 'E';
    info[1] = ':';

    for (unsigned int i = 0; i < size; i++) {
      info[i + LevelMarkSize] = text[i];
    }

    return info;
  }
};

}
#endif
