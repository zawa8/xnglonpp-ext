// Platform-independent core: UTF-8 in, UTF-8 out. No Win32/Notepad++
// dependency here on purpose, so it can be built and unit-tested on any
// platform (see tests/) -- only src/npp_plugin/ needs Windows.
//
// Ported from htrlib (https://github.com/zawa8/htrlib), specifically
// src/hsciistr/u10_to_xi52.ts and src/hsciistr/xnglo_post.ts, devanagari
// (u1_map) only so far. See ../../CLAUDE.md for what's ported and what
// isn't yet.
#pragma once
#include <string>

namespace xnglo {

// Full romanization (htrlib's xi38 / uL2xi52()). Devanagari runs are
// transliterated to xnglo's xi38 Latin scheme; anything outside the
// devanagari block (U+0900-U+097F) passes through unchanged.
std::string to_xi38(const std::string& utf8_input);

// Semi-transliteration (htrlib's u*38 family / uL2u38()). Devanagari
// LETTERS (consonants, independent vowels) are kept as the original
// character; devanagari MARKS (matras, anusvara, candrabindu, visarga,
// nukta signs) convert to their xi38 value; virama is dropped entirely.
// e.g. नमस्ते -> नमसतe (न/म/स/त stay devanagari, ् drops, े -> "e").
std::string to_u38(const std::string& utf8_input);

}  // namespace xnglo
