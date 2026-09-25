// Minimal test harness (no external framework -- keeps `tests/` buildable
// with a bare g++, no dependency fetching required). Mirrors a subset of
// htrlib's __tests__/hsciistr.test.ts assertions for the devanagari path,
// since this is a hand-port of that same logic and needs to match it.
#include "../src/xnglo_core/core.h"
#include <iostream>
#include <string>

static int g_failures = 0;

static void expect_eq(const std::string& label, const std::string& got, const std::string& want) {
  if (got != want) {
    std::cerr << "FAIL " << label << ": got \"" << got << "\" want \"" << want << "\"\n";
    ++g_failures;
  } else {
    std::cout << "ok   " << label << " -> \"" << got << "\"\n";
  }
}

int main() {
  using xnglo::to_xi38;
  using xnglo::to_u38;

  // xi38 (full romanization) -- ported straight from htrlib's tests.
  expect_eq("xi38(अनार)", to_xi38("अनार"), "Anar");
  expect_eq("xi38(नमस्ते)", to_xi38("नमस्ते"), "nmsTe");
  expect_eq("xi38(हल्दी के पानी में नहाना चाहिए)",
            to_xi38("हल्दी के पानी में नहाना चाहिए"),
            "HlDi ke pani me nHana caHiye");
  expect_eq("xi38(जाऊँ दुआ कई पढ़ाई कउआ)",
            to_xi38("जाऊँ दुआ कई पढ़ाई कउआ"),
            "zau Dua kyi pRai kAua");
  expect_eq("xi38(गएैसा गए आए हुए लिए)",
            to_xi38("गएैसा गए आए हुए लिए"),
            "gyesa gye aye Huye liye");
  // decomposed-nukta ढ़ (ढ + combining U+093C), not the precomposed form
  expect_eq("xi38(पढ़ाई, decomposed nukta)", to_xi38("पढ\u093Cाई"), "pRai");

  // u38 (semi-transliteration) -- per the repo owner's confirmed examples.
  expect_eq("u38(नमस्ते)", to_u38("नमस्ते"), "नमसतe");
  expect_eq("u38(अनार)", to_u38("अनार"), "अनaर");

  // xi38, other 5 scripts -- reusing the same smoke-test strings verified
  // against htrlib directly when each uN_map.ts was populated there.
  expect_eq("xi38 gurmukhi(ਸਤ ਸ੍ਰੀ ਅਕਾਲ ਪੰਜਾਬ)",
            to_xi38("ਸਤ ਸ੍ਰੀ ਅਕਾਲ ਪੰਜਾਬ"), "sT sri Akal pnzab");
  expect_eq("xi38 gujarati(કેમ છો ગુજરાત)",
            to_xi38("કેમ છો ગુજરાત"), "kem Co guzraT");
  // ओडिशा with the PRECOMPOSED nukta ଡ଼ (U+0B5C) -- decomposed nukta isn't
  // composed for any script but devanagari yet, see CLAUDE.md.
  expect_eq("xi38 oriya(ଓ\u0B5Cିଶା, precomposed nukta)",
            to_xi38("ଓ\u0B5Cିଶା"), "oRiSa");
  expect_eq("xi38 kannada(ನಮಸ್ಕಾರ ಕನ್ನಡ)",
            to_xi38("ನಮಸ್ಕಾರ ಕನ್ನಡ"), "nmskar knnd");
  expect_eq("xi38 sinhala(ආයුබෝවන්)", to_xi38("ආයුබෝවන්"), "Aayubown");

  // u38, gurmukhi -- ISCII-aligned, so the generic letter/mark split in
  // to_u38() should apply the same way it does for devanagari.
  expect_eq("u38 gurmukhi(ਸਤ)", to_u38("ਸਤ"), "ਸਤ"); // no marks, letters untouched
  expect_eq("u38 sinhala passthrough(ආයුබෝවන්)", to_u38("ආයුබෝවන්"), "ආයුබෝවන්");

  if (g_failures) {
    std::cerr << g_failures << " failure(s)\n";
    return 1;
  }
  std::cout << "all passed\n";
  return 0;
}
