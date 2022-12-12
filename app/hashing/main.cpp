#include "tracing/hash_trace.h"
#include "tracing/printer.h"
#include "tracing/trace.h"

#include <iostream>

using namespace tracing;

void writeOutput(const char character)
{
  std::cout << character;
}

int main(int argc, char* argv[])
{
  Printer printer;
  printer.registerOutput(writeOutput);

  /*
   * Simple printer
   */
  printer.print("# Simple printer #");

  printer.print("Kaczka dziwaczka");
  printer.print("Kaczka dziwaczka {} numer startowy", 10);
  uint32_t duck = 0x1234;
  printer.print("[:4]Czerwona[] Kaczka {:#x} dziwaczka {} numer startowy", duck, 0xA);
  printer.print("Czerwona Kaczka [:2]{:#b}[] dziwaczka", duck);

  printer.print("Kaczka dziwaczka numer startowy", 0xBABA);

  printer.print("");

  /*
   * Trace system without hashing
   */
  printer.print("# Trace system without hashing #");

  printer.print(Trace::info("Mleczna Krowa"));
  printer.print(Trace::warning("Mleczna {} Krowa"), 10);

  constexpr auto error = Trace::error("Mleczna {:#x} Krowa {}");
  printer.print(error, 0x3456, 0xA);

  printer.print("");

  /*
   * Trace system with hashing
   */
  printer.print("# Trace system with hashing #");

  printer.print(HashTrace::info("Byczy Byk"));
  printer.print(HashTrace::warning("Byczy {} Byk"), 10);

  constexpr auto hashError = HashTrace::error("Byczy {:#b} Byk {}");
  printer.print(hashError, 0x7890, 0xB);

  return 0;
}
