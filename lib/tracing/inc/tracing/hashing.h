#ifndef LIB_TRACING_HASHING_H
#define LIB_TRACING_HASHING_H

#include <array>
#include <bitset>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <iostream>

namespace tracing {

constexpr size_t Md5BlockSize = 512 / 8;
constexpr size_t Md5MsgLenBlockSize = 64 / 8;
constexpr size_t Md5HashLen = 128 / 8;

constexpr uint32_t BufferA0 = 0x67452301;
constexpr uint32_t BufferB0 = 0xEFCDAB89;
constexpr uint32_t BufferC0 = 0x98BADCFE;
constexpr uint32_t BufferD0 = 0x10325476;

constexpr uint32_t K[] = {
  0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
  0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,

  0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
  0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,

  0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
  0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,

  0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
  0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
};

constexpr uint32_t S[] = {
  7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,

  5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,

  4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,

  6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
};

using Md5Hash = std::array<uint8_t, Md5HashLen>;

constexpr size_t getBlocksNumber(const size_t size)
{
  return (size + Md5MsgLenBlockSize) / Md5BlockSize + 1;
}

constexpr size_t getPaddedMsgSize(const size_t size)
{
  return getBlocksNumber(size) * Md5BlockSize;
}

constexpr uint32_t encodeWord(const uint8_t* data)
{
  return (static_cast<uint32_t>(data[0])) | (static_cast<uint32_t>(data[1]) << 8) | (static_cast<uint32_t>(data[2]) << 16) |
         (static_cast<uint32_t>(data[3]) << 24);
}

constexpr std::array<uint8_t, sizeof(uint32_t)> decodeWord(const uint32_t word)
{
  std::array<uint8_t, sizeof(uint32_t)> data{};
  for (unsigned int i = 0; i < sizeof(uint32_t); i++) {
    data[i] = (word >> (i * 8)) & 0xFF;
  }
  return data;
}

template<size_t size>
constexpr std::array<uint8_t, size - 1> removeNullTerminator(const char (&msg)[size])
{
  std::array<uint8_t, size - 1> data{};
  for (unsigned int i = 0; i < size - 1; i++) {
    data[i] = msg[i];
  }
  return data;
}

template<size_t size>
constexpr std::array<uint8_t, getPaddedMsgSize(size)> doMsgPadding(const std::array<uint8_t, size>& msg)
{
  constexpr auto paddedMsgSize = getPaddedMsgSize(size);
  constexpr auto paddingBeginOffset = size;
  constexpr auto paddingEndOffset = paddedMsgSize - Md5MsgLenBlockSize;

  std::array<uint8_t, paddedMsgSize> data{};
  for (unsigned int i = 0; i < msg.size(); i++) {
    data[i] = msg[i];
  }

  data[paddingBeginOffset] = 0x80;

  for (unsigned int i = paddingBeginOffset + 1; i < paddingEndOffset; i++) {
    data[i] = 0x00;
  }

  constexpr uint64_t msgSize = size * 8;
  for (unsigned int i = 0; i < (Md5MsgLenBlockSize / sizeof(uint32_t)); i++) {
    auto bytes = decodeWord(msgSize >> (32 * i));
    for (unsigned int j = 0; j < bytes.size(); j++) {
      data[paddingEndOffset + ((i * bytes.size()) + j)] = bytes[j];
    }
  }

  return data;
}

template<size_t size>
constexpr std::array<uint32_t, size / sizeof(uint32_t)> convertToWords(const std::array<uint8_t, size>& data)
{
  constexpr size_t wordsNumber = size / sizeof(uint32_t);
  std::array<uint32_t, wordsNumber> words{};
  for (unsigned int i = 0; i < wordsNumber; i++) {
    words[i] = encodeWord(&data[i * 4]);
  }
  return words;
}

constexpr uint32_t rotateLeft(uint32_t value, size_t amount)
{
  auto mask = (1 << amount) - 1;
  return ((value >> (32 - amount)) & mask) | ((value << amount) & ~mask);
}

template<size_t size>
constexpr Md5Hash md5(const std::array<unsigned char, size>& text)
{
  const auto data = doMsgPadding(text);
  const auto words = convertToWords(data);

  uint32_t a0 = BufferA0;
  uint32_t b0 = BufferB0;
  uint32_t c0 = BufferC0;
  uint32_t d0 = BufferD0;
  uint32_t f, g;

  for (unsigned int block = 0; block < getBlocksNumber(text.size()); block++) {
    uint32_t a = a0;
    uint32_t b = b0;
    uint32_t c = c0;
    uint32_t d = d0;
    for (unsigned int i = 0; i < 64; i++) {
      if (i < 16) {
        f = (b & c) | (~b & d);
        g = i;
      } else if (i < 32) {
        f = (d & b) | (~d & c);
        g = (5 * i + 1) % 16;
      } else if (i < 48) {
        f = b ^ c ^ d;
        g = (3 * i + 5) % 16;
      } else {
        f = c ^ (b | ~d);
        g = (7 * i) % 16;
      }
      g = g + (block * 16);
      f = f + a + K[i] + words[g];
      a = d;
      d = c;
      c = b;
      b = b + rotateLeft(f, S[i]);
    }
    a0 = a0 + a;
    b0 = b0 + b;
    c0 = c0 + c;
    d0 = d0 + d;
  }
  std::array<uint32_t, 4> resultInWords = { a0, b0, c0, d0 };
  Md5Hash result{};
  for (unsigned int i = 0; i < result.size(); i += 4) {
    auto bytes = decodeWord(resultInWords[i / 4]);
    for (unsigned int j = 0; j < bytes.size(); j++) {
      result[i + j] = bytes[j];
    }
  }
  return result;
}

template<size_t size>
constexpr Md5Hash md5(const char (&text)[size])
{
  const auto msg = removeNullTerminator(text);
  return md5(msg);
}

static_assert(md5(std::array<unsigned char, 1>{ 'a' }) ==
              Md5Hash{ 0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8, 0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 });
static_assert(md5("") == Md5Hash{ 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e });
static_assert(md5("a") == Md5Hash{ 0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8, 0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 });
static_assert(md5("abc") == Md5Hash{ 0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72 });
static_assert(md5("message digest") ==
              Md5Hash{ 0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d, 0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0 });
static_assert(md5("abcdefghijklmnopqrstuvwxyz") ==
              Md5Hash{ 0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00, 0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b });
static_assert(md5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") ==
              Md5Hash{ 0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5, 0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f });
static_assert(md5("12345678901234567890123456789012345678901234567890123456789012345678901234567890") ==
              Md5Hash{ 0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55, 0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a });

};

#endif /* LIB_TRACING_HASHING_H */
