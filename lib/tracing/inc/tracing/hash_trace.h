#ifndef LIB_TRACING_HASH_TRACE_H
#define LIB_TRACING_HASH_TRACE_H

#include "tracing/hashing.h"

#include <array>
#include <cstdint>
#include <cstring>

namespace tracing {

class HashTrace
{
private:
  static constexpr size_t LevelMarkSize = 2U;
  static constexpr size_t HashTraceSize = (Md5HashLen * 2) + 1;
  static constexpr size_t AsciiCharPerByte = 2U;

private:
  static constexpr std::array<char, AsciiCharPerByte> toAscii(const uint8_t byte)
  {
    constexpr char ascii[] = "0123456789abcdef";
    std::array<char, AsciiCharPerByte> output{ ascii[byte >> 4], ascii[byte & 0xF] };
    return output;
  }

  template<size_t size>
  static constexpr std::array<unsigned char, HashTraceSize> hashing(std::array<unsigned char, size> text)
  {
    std::array<unsigned char, HashTraceSize> output{};
    auto hash = md5(text);
    for (unsigned int i = 0; i < Md5HashLen; i++) {
      auto ascii = toAscii(hash[i]);
      for (unsigned int j = 0; j < AsciiCharPerByte; j++) {
        output[(i * AsciiCharPerByte) + j] = ascii[j];
      }
    }
    return output;
  }

public:
  template<size_t size>
  static constexpr std::array<unsigned char, HashTraceSize> info(const char (&text)[size])
  {
    std::array<unsigned char, (size + HashTrace::LevelMarkSize - 1)> info{};
    info[0] = 'I';
    info[1] = ':';

    for (unsigned int i = 0; i < (size - 1); i++) {
      info[i + LevelMarkSize] = text[i];
    }

    return hashing(info);
  }

  template<size_t size>
  static constexpr std::array<unsigned char, HashTraceSize> warning(const char (&text)[size])
  {
    std::array<unsigned char, size + HashTrace::LevelMarkSize - 1> warning{};
    warning[0] = 'W';
    warning[1] = ':';

    for (unsigned int i = 0; i < (size - 1); i++) {
      warning[i + LevelMarkSize] = text[i];
    }

    return hashing(warning);
  }

  template<size_t size>
  static constexpr std::array<unsigned char, HashTraceSize> error(const char (&text)[size])
  {
    std::array<unsigned char, size + HashTrace::LevelMarkSize - 1> error{};
    error[0] = 'E';
    error[1] = ':';

    for (unsigned int i = 0; i < (size - 1); i++) {
      error[i + LevelMarkSize] = text[i];
    }

    return hashing(error);
  }
};

}
#endif /* LIB_TRACING_HASH_TRACE_H */
