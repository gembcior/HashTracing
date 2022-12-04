#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include "hash.h"

using namespace std;
using namespace hashing;

int main(int argc, char *argv[])
{
  // constexpr Md5Hash hash0 = md5("They are deterministic");
  Md5Hash hash0 = md5("They are deterministic");
  // cout << "#TG: 0x" << hex << hash0 << endl;

  // constexpr Hash hash1 = md5("This is a very long message to test bigger MD5 block size. It must be longer than 64 byte.");
  // Md5Hash hash1 = md5("This is a very long message to test bigger MD5 block size. It must be longer than 64 byte.");
  // cout << "#TG: 0x" << hex << hash1 << endl;

  return 0;
}
