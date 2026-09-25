// Ported from htrlib (https://github.com/zawa8/htrlib)
// src/hsciistr/dicts/u4_map.ts -- gujarati, block U+0A80-U+0AFF.
// Keep this in sync by hand if htrlib's u4_map.ts changes; there is no
// automated sync between the two repos.
#pragma once
#include <array>
#include <string>

namespace xnglo {

// index = codepoint - 0x0A80 (offset within the gujarati block).
inline const std::array<std::string, 128>& u4_map() {
  static const std::array<std::string, 128> table = { {
    "", "N", "N", ":", "", "A", "a", "_i", "_i", "_u", "_u", "ri", "li",
    "_e", "", "_e", "_e", "ao", "", "o", "ou", "k", "K", "g", "G", "N",
    "c", "C", "z", "Z", "n", "t", "th", "d", "dh", "n", "T", "Th", "D",
    "Dh", "n", "", "p", "f", "b", "B", "m", "y", "r", "", "l", "l", "",
    "w", "S", "s", "s", "H", "", "", "", "!", "a", "i", "i", "u", "u",
    "ri", "r", "e", "", "e", "ye", "o", "", "o", "ou", "", "", "", "om",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "ri",
    "li", "li", "li", "", "", "0", "1", "2", "3", "4", "5", "6", "7", "8",
    "9", "", "", "", "", "", "", "", "", "", "Z", "", "", "", "", "", ""
  } };
  return table;
}

constexpr int kGujaratiBase = 0x0A80;
constexpr int kGujaratiViramaOffset = 0x4d;

}  // namespace xnglo
