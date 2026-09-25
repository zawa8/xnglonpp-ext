#include "core.h"
#include "u1_map.h"
#include "u3_map.h"
#include "u4_map.h"
#include "u5_map.h"
#include "u8_map.h"
#include "u10_map.h"

#include <array>
#include <regex>
#include <vector>

namespace xnglo {
namespace {

// ---- minimal UTF-8 <-> UTF-32 codepoint helpers -----------------------
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
    else { out.push_back(c); ++i; continue; }
    if (i + len > n) { out.push_back(c); ++i; continue; }
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

// ---- script registry ---------------------------------------------------
// `iscii_aligned` marks whether the script shares devanagari's ISCII
// layout closely enough that to_u38()'s generic letter/mark range check
// applies -- true for the 5 non-sinhala scripts here; sinhala genuinely
// diverges (extra independent vowels shift everything after them -- see
// u10_map.h's own htrlib source note) so to_u38() doesn't support it.
struct ScriptTable {
  int base;
  int virama_offset;
  bool iscii_aligned;
  const std::array<std::string, 128>& map;
};

const std::array<ScriptTable, 6>& scripts() {
  static const std::array<ScriptTable, 6> table = { {
    { kBlockBase,      kViramaOffset,        true,  u1_map()  },
    { kGurmukhiBase,   kGurmukhiViramaOffset, true,  u3_map()  },
    { kGujaratiBase,   kGujaratiViramaOffset, true,  u4_map()  },
    { kOriyaBase,      kOriyaViramaOffset,    true,  u5_map()  },
    { kKannadaBase,    kKannadaViramaOffset,  true,  u8_map()  },
    { kSinhalaBase,    kSinhalaViramaOffset,  false, u10_map() },
  } };
  return table;
}

const ScriptTable* table_for(char32_t cp) {
  for (const auto& t : scripts()) {
    if (cp >= static_cast<char32_t>(t.base) && cp < static_cast<char32_t>(t.base) + 128) {
      return &t;
    }
  }
  return nullptr;
}

// ---- devanagari-specific preprocessing (ported from u10_to_xi52.ts) ---
// Both of these only ever match literal devanagari codepoints, so they're
// safe (no-ops) on input from any other script -- but that also means
// they DON'T do the equivalent cleanup for the other 5 scripts' own
// nukta/malformed-vowel-matra quirks yet. See CLAUDE.md.
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

void drop_malformed_vowel_matra(std::vector<char32_t>& cps) {
  std::vector<char32_t> out;
  out.reserve(cps.size());
  for (size_t i = 0; i < cps.size(); ++i) {
    if (cps[i] >= 0x0904 && cps[i] <= 0x0914 &&
        i + 1 < cps.size() && cps[i + 1] >= 0x093E && cps[i + 1] <= 0x094C) {
      continue;
    }
    out.push_back(cps[i]);
  }
  cps = std::move(out);
}

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
// Only used by to_xi38(); to_u38() intentionally skips this.
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

  std::string out;
  for (char32_t cp : cps) {
    const ScriptTable* t = table_for(cp);
    if (t) {
      out += t->map[cp - t->base];
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

  std::string out;
  for (char32_t cp : cps) {
    const ScriptTable* t = table_for(cp);
    if (!t || !t->iscii_aligned) {
      utf8_append(out, cp);
      continue;
    }
    int offset = static_cast<int>(cp) - t->base;
    if (offset == t->virama_offset) continue;
    bool is_letter =
        (offset >= 0x04 && offset <= 0x39) ||
        (offset >= 0x58 && offset <= 0x61) ||
        offset == 0x7F;
    if (is_letter) {
      utf8_append(out, cp);
    } else {
      out += t->map[offset];
    }
  }
  return out;
}

}  // namespace xnglo
