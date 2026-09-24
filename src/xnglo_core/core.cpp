#include "core.h"
#include "u1_map.h"

#include <regex>
#include <vector>

namespace xnglo {
namespace {

// ---- minimal UTF-8 <-> UTF-32 codepoint helpers -----------------------
// (devanagari is entirely in the 3-byte UTF-8 range; this only needs to
// handle 1..3 byte sequences correctly, but decodes 4-byte ones too so
// it doesn't corrupt astral-plane input it passes through unchanged.)

std::vector<char32_t> utf8_decode(const std::string& s) {
  std::vector<char32_t> out;
  size_t i = 0, n = s.size();
  while (i < n) {
    unsigned char c = static_cast<unsigned char>(s[i]);
    char32_t cp;
    size_t len;
    if ((c & 0x80) == 0x00) { cp = c; len = 1; }
    else if ((c & 0xE0) == 0xC0) { cp = c & 0x1F; len = 2; }
    else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; len = 3; }
    else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; len = 4; }
    else { out.push_back(c); ++i; continue; } // invalid lead byte, passthrough raw
    if (i + len > n) { out.push_back(c); ++i; continue; } // truncated, passthrough raw
    bool ok = true;
    for (size_t k = 1; k < len; ++k) {
      unsigned char cc = static_cast<unsigned char>(s[i + k]);
      if ((cc & 0xC0) != 0x80) { ok = false; break; }
      cp = (cp << 6) | (cc & 0x3F);
    }
    if (!ok) { out.push_back(c); ++i; continue; }
    out.push_back(cp);
    i += len;
  }
  return out;
}

void utf8_append(std::string& out, char32_t cp) {
  if (cp <= 0x7F) {
    out += static_cast<char>(cp);
  } else if (cp <= 0x7FF) {
    out += static_cast<char>(0xC0 | (cp >> 6));
    out += static_cast<char>(0x80 | (cp & 0x3F));
  } else if (cp <= 0xFFFF) {
    out += static_cast<char>(0xE0 | (cp >> 12));
    out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
    out += static_cast<char>(0x80 | (cp & 0x3F));
  } else {
    out += static_cast<char>(0xF0 | (cp >> 18));
    out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
    out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
    out += static_cast<char>(0x80 | (cp & 0x3F));
  }
}

// ---- devanagari-specific preprocessing (ported from u10_to_xi52.ts) ---

// Composes the 8 nukta consonants (क़ ख़ ग़ ज़ ड़ ढ़ फ़ य़) when the input
// gives them as decomposed base-letter + U+093C (not a canonical Unicode
// decomposition, so normalize('NFC') can't do this -- has to be by hand).
void compose_nukta(std::vector<char32_t>& cps) {
  static const std::vector<std::pair<char32_t, char32_t>> pairs = {
    {0x0915, 0x0958}, {0x0916, 0x0959}, {0x0917, 0x095A}, {0x091C, 0x095B},
    {0x0921, 0x095C}, {0x0922, 0x095D}, {0x092B, 0x095E}, {0x092F, 0x095F},
  };
  std::vector<char32_t> out;
  out.reserve(cps.size());
  for (size_t i = 0; i < cps.size(); ++i) {
    if (i + 1 < cps.size() && cps[i + 1] == 0x093C) {
      bool matched = false;
      for (auto& p : pairs) {
        if (cps[i] == p.first) { out.push_back(p.second); ++i; matched = true; break; }
      }
      if (matched) continue;
    }
    out.push_back(cps[i]);
  }
  cps = std::move(out);
}

// An independent vowel letter (U+0904-U+0914) directly followed by a
// dependent matra (U+093E-U+094C) is malformed -- a matra only attaches
// to a consonant -- and in practice is someone typing an extra vowel
// letter by mistake right before the matra they meant. Drop the
// spurious independent vowel, keep the matra.
void drop_malformed_vowel_matra(std::vector<char32_t>& cps) {
  std::vector<char32_t> out;
  out.reserve(cps.size());
  for (size_t i = 0; i < cps.size(); ++i) {
    if (cps[i] >= 0x0904 && cps[i] <= 0x0914 &&
        i + 1 < cps.size() && cps[i + 1] >= 0x093E && cps[i + 1] <= 0x094C) {
      continue; // drop this one, the matra right after it gets pushed next iteration
    }
    out.push_back(cps[i]);
  }
  cps = std::move(out);
}

// क्ष -> s, ज्ञ -> gy (whole-conjunct special cases, ported as-is).
// Operates on the UTF-8 string before the main per-codepoint loop,
// same as u10_to_xi52.ts does.
std::string apply_conjunct_specials(const std::string& utf8) {
  static const std::regex kshaWordStart("^\u0915\u094D\u0937");
  static const std::regex kshaMid("(\\W)\u0915\u094D\u0937");
  static const std::regex gya("\u091C\u094D\u091E");
  std::string s = std::regex_replace(utf8, kshaWordStart, "s");
  s = std::regex_replace(s, kshaMid, "$1s");
  s = std::regex_replace(s, gya, "gy");
  return s;
}

// ---- postprocessing, ported from xnglo_post.ts's xnglo_india_post() ---
// Only used by to_xi38(); to_u38() intentionally skips this (matches
// htrlib's unicode_india_to_u38(), which returns the raw per-character
// result unprocessed).
std::string xnglo_india_post(const std::string& in) {
  std::string s = in;
  s = std::regex_replace(s, std::regex("^#S"), "S");
  s = std::regex_replace(s, std::regex("(\\W)#S"), "$1S");
  s = std::regex_replace(s, std::regex("#S"), "kS");
  s = std::regex_replace(s, std::regex("^_"), "");
  s = std::regex_replace(s, std::regex("(\\W)_"), "$1");
  s = std::regex_replace(s, std::regex("([^\\Waiueo_])_u"), "$1Au");
  s = std::regex_replace(s, std::regex("([^\\Waiueo_])_o"), "$1Ao");
  s = std::regex_replace(s, std::regex("a_i"), "ai");
  s = std::regex_replace(s, std::regex("a_u"), "au");
  s = std::regex_replace(s, std::regex("a_o"), "ao");
  s = std::regex_replace(s, std::regex("_i"), "yi");
  s = std::regex_replace(s, std::regex("_e"), "ye");
  s = std::regex_replace(s, std::regex("_u"), "xu");
  s = std::regex_replace(s, std::regex("_o"), "xo");
  s = std::regex_replace(s, std::regex("N$"), "");
  s = std::regex_replace(s, std::regex("N(\\W)"), "$1");
  s = std::regex_replace(s, std::regex("Nb"), "mb");
  s = std::regex_replace(s, std::regex("NB"), "mB");
  s = std::regex_replace(s, std::regex("Np"), "mp");
  s = std::regex_replace(s, std::regex("Nf"), "mf");
  s = std::regex_replace(s, std::regex("N(?![kKgG])"), "n");
  s = std::regex_replace(s, std::regex("([^kgcztdjqpbs])v"), "$1h");
  return s;
}

}  // namespace

std::string to_xi38(const std::string& utf8_input) {
  if (utf8_input.empty()) return "";
  std::string s = apply_conjunct_specials(utf8_input);
  std::vector<char32_t> cps = utf8_decode(s);
  drop_malformed_vowel_matra(cps);
  compose_nukta(cps);

  const auto& map = u1_map();
  std::string out;
  for (char32_t cp : cps) {
    if (cp >= kBlockBase && cp < kBlockBase + 128) {
      out += map[cp - kBlockBase];
    } else {
      utf8_append(out, cp);
    }
  }
  return xnglo_india_post(out);
}

std::string to_u38(const std::string& utf8_input) {
  if (utf8_input.empty()) return "";
  std::string s = apply_conjunct_specials(utf8_input);
  std::vector<char32_t> cps = utf8_decode(s);
  drop_malformed_vowel_matra(cps);
  compose_nukta(cps);

  const auto& map = u1_map();
  std::string out;
  for (char32_t cp : cps) {
    if (cp < kBlockBase || cp >= kBlockBase + 128) {
      utf8_append(out, cp);
      continue;
    }
    int offset = static_cast<int>(cp - kBlockBase);
    if (offset == kViramaOffset) continue; // drop virama entirely
    // "Letter" here means: devanagari LETTER category (independent
    // vowels 0x04-0x14, consonants 0x15-0x39 roughly, nukta consonants
    // 0x58-0x5f) as opposed to combining marks (matras, anusvara,
    // candrabindu, visarga, nukta sign itself). Mirrors JS's \p{L} test
    // in htrlib's unicode_india_to_u38(), done here as an explicit
    // codepoint-range check since std::regex has no \p{L}.
    bool is_letter =
        (cp >= 0x0904 && cp <= 0x0939) ||             // independent vowels..ह
        (cp >= 0x0958 && cp <= 0x0961) ||              // nukta letters + ॠॡ
        cp == 0x097F;                                  // (rare extra letter)
    if (is_letter) {
      utf8_append(out, cp);
    } else {
      out += map[offset];
    }
  }
  return out;
}

}  // namespace xnglo
