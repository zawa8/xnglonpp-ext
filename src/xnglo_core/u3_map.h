// Ported from htrlib (https://github.com/zawa8/htrlib)
// src/hsciistr/dicts/u3_map.ts -- gurmukhi (punjabi), block U+0A00-U+0A7F.
// Keep this in sync by hand if htrlib's u3_map.ts changes; there is no
// automated sync between the two repos.
#pragma once
#include <array>
#include <string>

namespace xnglo {

// index = codepoint - 0x0A00 (offset within the gurmukhi (punjabi) block).
inline const std::array<std::string, 128>& u3_map() {
  static const std::array<std::string, 128> table = { {
    "", "N", "N", ":", "", "A", "a", "_i", "_i", "_u", "_u", "", "", "",
    "", "_e", "_e", "", "", "o", "ou", "k", "K", "g", "G", "N", "c", "C",
    "z", "Z", "n", "t", "th", "d", "dh", "n", "T", "Th", "D", "Dh", "n",
    "", "p", "f", "b", "B", "m", "y", "r", "", "l", "l", "", "w", "S", "",
    "s", "H", "", "", "", "", "a", "i", "i", "u", "u", "", "", "", "",
    "e", "ye", "", "", "o", "ou", "", "", "", "", "", "", "", "", "", "",
    "", "", "K", "g", "z", "R", "", "f", "", "", "", "", "", "", "", "0",
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "N", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", ""
  } };
  return table;
}

constexpr int kGurmukhiBase = 0x0A00;
constexpr int kGurmukhiViramaOffset = 0x4d;

}  // namespace xnglo
