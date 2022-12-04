#ifndef HASH_H
#define HASH_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>

namespace hashing {

template<typename T, size_t size>
void showTheArray(std::array<T, size> data)
{
  for (auto& item : data) {
    std::cout << std::hex << static_cast<uint32_t>(item) << " : ";
  }
  std::cout << std::endl;
  std::cout << std::dec << "Size: " << data.size() << std::endl;
}

constexpr size_t Md5BlockSize = 512 / 8;
constexpr size_t Md5MsgLenBlockSize = 64 / 8;
constexpr size_t Md5HashLen = 128 / 8;

using Md5Hash = std::array<uint32_t, Md5HashLen / sizeof(uint32_t)>;

constexpr size_t getPaddedMsgSize(const size_t size)
{
  const size_t blocksNumber = (size + Md5MsgLenBlockSize) / Md5BlockSize + 1;
  return blocksNumber * Md5BlockSize;
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
constexpr std::array<uint8_t, getPaddedMsgSize(size)> doMsgPadding(std::array<uint8_t, size>& msg)
{
  constexpr auto paddedMsgSize = getPaddedMsgSize(size);
  constexpr auto paddingBeginOffset = size;
  constexpr auto paddingEndOffset = paddedMsgSize - Md5MsgLenBlockSize;

  std::array<uint8_t, paddedMsgSize> data{};
  for (unsigned int i = 0; i < size; i++) {
    data[i] = msg[i];
  }

  data[paddingBeginOffset] = 0x80;

  for (unsigned int i = paddingBeginOffset + 1; i < paddingEndOffset; i++) {
    data[i] = 0x00;
  }

  constexpr uint64_t msgSize = size * 8;
  for (unsigned int i = 0; i < Md5MsgLenBlockSize; i++) {
    const unsigned int position = 8 * (i + 1);
    data[paddingEndOffset + i] = (msgSize >> position) & 0xFF;
  }

  return data;
}

template<size_t size>
constexpr std::array<uint32_t, size / sizeof(uint32_t)> convertToWords(std::array<uint8_t, size> data)
{
  constexpr size_t wordsNumber = size / sizeof(uint32_t);
  std::array<uint32_t, wordsNumber> words{};
  for (unsigned int i = 0; i < wordsNumber; i++) {
    words[i] = (static_cast<uint32_t>(data[i * 4]) << 24) | (static_cast<uint32_t>(data[i * 4 + 1]) << 16) |
               (static_cast<uint32_t>(data[i * 4 + 2]) << 8) | (static_cast<uint32_t>(data[i * 4 + 3]));
  }
  return words;
}

template<size_t size>
constexpr Md5Hash md5(const char (&text)[size])
{
  auto msg = removeNullTerminator(text);
  showTheArray(msg);
  auto data = doMsgPadding(msg);
  showTheArray(data);
  auto words = convertToWords(data);
  showTheArray(words);

  Md5Hash result{};

  return result;
}

};

#endif /* HASH_H */
