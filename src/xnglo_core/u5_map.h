// Ported from htrlib (https://github.com/zawa8/htrlib)
// src/hsciistr/dicts/u5_map.ts -- oriya, block U+0B00-U+0B7F.
// Keep this in sync by hand if htrlib's u5_map.ts changes; there is no
// automated sync between the two repos.
#pragma once
#include <array>
#include <string>

namespace xnglo {

// index = codepoint - 0x0B00 (offset within the oriya block).
inline const std::array<std::string, 128>& u5_map() {
  static const std::array<std::string, 128> table = { {
    "", "N", "N", ":", "", "A", "a", "_i", "_i", "_u", "_u", "ri", "li",
    "", "", "_e", "_e", "", "", "o", "ou", "k", "K", "g", "G", "N", "c",
    "C", "z", "Z", "n", "t", "th", "d", "dh", "n", "T", "Th", "D", "Dh",
    "n", "", "p", "f", "b", "B", "m", "y", "r", "", "l", "l", "", "w",
    "S", "s", "s", "H", "", "", "", "!", "a", "i", "i", "u", "u", "ri",
    "r", "", "", "e", "ye", "", "", "o", "ou", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "R", "R", "", "y", "ri", "li", "li",
    "li", "", "", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "",
    "w", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
  } };
  return table;
}

constexpr int kOriyaBase = 0x0B00;
constexpr int kOriyaViramaOffset = 0x4d;

}  // namespace xnglo
