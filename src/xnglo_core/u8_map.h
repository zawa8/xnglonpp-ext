// Ported from htrlib (https://github.com/zawa8/htrlib)
// src/hsciistr/dicts/u8_map.ts -- kannada, block U+0C80-U+0CFF.
// Keep this in sync by hand if htrlib's u8_map.ts changes; there is no
// automated sync between the two repos.
#pragma once
#include <array>
#include <string>

namespace xnglo {

// index = codepoint - 0x0C80 (offset within the kannada block).
inline const std::array<std::string, 128>& u8_map() {
  static const std::array<std::string, 128> table = { {
    "N", "N", "N", ":", "", "A", "a", "_i", "_i", "_u", "_u", "ri", "li",
    "", "_e", "_e", "_e", "", "_o", "o", "ou", "k", "K", "g", "G", "N",
    "c", "C", "z", "Z", "n", "t", "th", "d", "dh", "n", "T", "Th", "D",
    "Dh", "n", "", "p", "f", "b", "B", "m", "y", "r", "r", "l", "l", "",
    "w", "S", "s", "s", "H", "", "", "", "!", "a", "i", "i", "u", "u",
    "ri", "r", "", "e", "e", "ye", "", "oe", "o", "ou", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "n", "f", "", "ri",
    "li", "li", "li", "", "", "0", "1", "2", "3", "4", "5", "6", "7", "8",
    "9", "", "", "", "N", "", "", "", "", "", "", "", "", "", "", "", ""
  } };
  return table;
}

constexpr int kKannadaBase = 0x0C80;
constexpr int kKannadaViramaOffset = 0x4d;

}  // namespace xnglo
