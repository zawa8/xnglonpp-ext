// Ported from htrlib (https://github.com/zawa8/htrlib)
// src/hsciistr/dicts/u1_map.ts -- devanagari, block U+0900-U+097F.
// Keep this in sync by hand if htrlib's u1_map.ts changes; there is no
// automated sync between the two repos.
#pragma once
#include <array>
#include <string>

namespace xnglo {

// index = codepoint - 0x0900 (offset within the devanagari block).
inline const std::array<std::string, 128>& u1_map() {
  static const std::array<std::string, 128> table = { {
    "", "N", "N", ":", "Ae", "A", "a", "_i", "_i", "_u", "_u", "ri", "li",
    "_e", "_e", "_e", "_e", "ao", "_o", "o", "ou", "k", "K", "g", "G", "N",
    "c", "C", "z", "Z", "n", "t", "th", "d", "dh", "n", "T", "Th", "D",
    "Dh", "n", "n", "p", "f", "b", "B", "m", "y", "r", "r", "l", "l", "l",
    "w", "S", "s", "s", "H", "oe", "ui", "", "!", "a", "i", "i", "u", "u",
    "ri", "r", "e", "e", "e", "ye", "o", "oe", "o", "ou", "", "", "ou",
    "om", "", "", "`", "'", "eei", "ui", "uui", "k", "K", "g", "z", "R",
    "R", "f", "y", "ri", "li", "li", "li", ".", ".", "0", "1", "2", "3",
    "4", "5", "6", "7", "8", "9", "_", "__", "A", "Ao", "Ao", "Ao", "ui",
    "ui", "D", "Z", "y", "n", "z", "?", "d", "b"
  } };
  return table;
}

// Devanagari's virama sits at this offset within the block (0x0900 + 0x4d
// = U+094D SIGN VIRAMA). Sinhala (not ported here yet) uses 0x4a instead
// -- see htrlib's u10_to_xi52.ts VIRAMA_OFFSET table if/when that's added.
constexpr int kViramaOffset = 0x4d;
constexpr int kBlockBase = 0x0900;

}  // namespace xnglo
