// Ported from htrlib (https://github.com/zawa8/htrlib)
// src/hsciistr/dicts/u10_map.ts -- sinhala, block U+0D80-U+0DFF.
// Keep this in sync by hand if htrlib's u10_map.ts changes; there is no
// automated sync between the two repos.
#pragma once
#include <array>
#include <string>

namespace xnglo {

// index = codepoint - 0x0D80 (offset within the sinhala block).
inline const std::array<std::string, 128>& u10_map() {
  static const std::array<std::string, 128> table = { {
    "", "N", "N", ":", "e", "A", "Aa", "AE", "ae", "_i", "_i", "_u", "_u",
    "r", "ri", "l", "li", "_e", "_e", "_e", "_o", "_o", "_o", "", "", "",
    "k", "K", "g", "gh", "N", "N", "ch", "Ch", "z", "Z", "n", "n", "n",
    "t", "th", "d", "dh", "n", "n", "T", "Th", "D", "Dh", "n", "", "nq",
    "p", "f", "b", "B", "m", "mb", "y", "r", "", "l", "", "", "w", "S",
    "s", "s", "H", "l", "f", "", "", "", "", "", "", "", "", "a", "e",
    "ae", "i", "i", "u", "", "u", "", "ri", "e", "e", "ye", "o", "o", "o",
    "l", "", "", "", "", "", "", "0", "1", "2", "3", "4", "5", "6", "7",
    "8", "9", "", "", "ri", "li", ".", "", "", "", "", "", "", "", "", "",
    "", ""
  } };
  return table;
}

constexpr int kSinhalaBase = 0x0D80;
constexpr int kSinhalaViramaOffset = 0x4a;

}  // namespace xnglo
