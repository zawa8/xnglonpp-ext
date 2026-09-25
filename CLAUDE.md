# htr-xnglo Notepad++ plugin (xnglonpp-ext)

Transliterates selected text in Notepad++ using htrlib's xnglo scheme
(https://github.com/zawa8/htrlib). Supports 6 scripts so far: devanagari,
gurmukhi (punjabi), gujarati, oriya, kannada, sinhala -- see "What's NOT
done yet" for the other 4 (bengali, tamil, telugu, malayalam), which
htrlib itself doesn't have verified per-script data for yet either. Two
menu commands:
- "Transliterate selection -> xi38 (full romanization)" -- e.g. नमस्ते -> nmsTe
- "Transliterate selection -> u38 (keep native letters)" -- e.g. नमस्ते -> नमसतe
  (letters stay native-script, only matras/marks convert, virama drops)

## Layout
- `src/xnglo_core/` -- platform-independent C++ (no Win32/Notepad++
  dependency). Hand-ported from htrlib's TypeScript (`src/hsciistr/
  u10_to_xi52.ts` + `xnglo_post.ts`). 6 scripts ported: devanagari
  (`u1_map.h`), gurmukhi (`u3_map.h`), gujarati (`u4_map.h`), oriya
  (`u5_map.h`), kannada (`u8_map.h`), sinhala (`u10_map.h`) -- the ones
  htrlib itself has verified per-script data for. Dispatch across them
  lives in `core.cpp`'s `ScriptTable`/`scripts()`/`table_for()`.
  `to_u38()` (semi-transliteration) only supports the 5 ISCII-aligned
  ones (not sinhala, which doesn't share devanagari's letter/mark offset
  layout -- see `ScriptTable::iscii_aligned`). Builds and runs with a
  bare `g++` on any platform, including this repo's Linux sandbox -- see
  `tests/`.
- `src/npp_plugin/` -- Win32 + Notepad++ Plugin API glue (menu
  registration, reading/replacing the Scintilla selection, DLL exports).
  Based on the structure of Notepad++'s official plugin template
  (https://github.com/npp-plugins/plugintemplate). **Windows-only, and
  UNCOMPILED/UNTESTED** -- this sandbox has no Windows SDK, MSVC, or a
  real Notepad++ to load the DLL into. Written carefully against the
  documented plugin contract, but treat it as a first draft that needs a
  real Windows build + manual test in Notepad++ before trusting it.
- `tests/core_test.cpp` -- a small hand-rolled test harness (no external
  framework, so `tests/` stays buildable with nothing but g++) exercising
  `xnglo_core` against the same assertions as htrlib's own
  `__tests__/hsciistr.test.ts`, since this is a hand-port of that logic
  and needs to keep matching it.

Build/run the portable core's tests:
```
g++ -std=c++17 -Wall -o /tmp/core_test src/xnglo_core/core.cpp tests/core_test.cpp
/tmp/core_test
```

Building the actual Notepad++ plugin DLL needs a Windows machine with
Visual Studio + the Notepad++ plugin template project settings (link
`src/npp_plugin/*.cpp` + `src/xnglo_core/core.cpp` together, x86 or x64
matching the target Notepad++ build). Not attempted here.

## What's NOT done yet
- 4 scripts still unported: bengali, tamil, telugu, malayalam -- htrlib
  itself doesn't have verified per-script data for these yet (its
  u2/u6/u7/u9_map.ts are still Devanagari-derived placeholders), so
  there's nothing correct to port until that happens there first. Adding
  one once it is: pull that script's `uN_map.ts` array out of htrlib
  (same way `u1_map.h`/`u3_map.h`/etc were generated -- a straight
  literal copy of the raw value array) into a new header, add its
  `kXBase`/`kXViramaOffset` constants and an entry in `core.cpp`'s
  `scripts()` table.
- `compose_nukta()` and `drop_malformed_vowel_matra()` in `core.cpp` are
  still devanagari-only (match literal devanagari codepoints) -- they're
  harmless no-ops on the other 5 scripts' text, but don't do the
  equivalent cleanup for THEIR nukta-decomposition or malformed-vowel-
  matra quirks. `oriya`'s test above uses the precomposed nukta form for
  exactly this reason (decomposed ଡ଼ currently reads as plain ଡ, same
  class of bug documented in htrlib's own u5_map.ts commit history).
- No keyboard shortcuts assigned to the two menu commands
  (`_pShKey = nullptr` in `PluginDefinition.cpp`) -- add a `ShortcutKey`
  struct there if wanted.
- No `.vcxproj`/CMake build files for the actual Windows DLL build --
  someone building this needs to wire `src/npp_plugin/*.cpp` +
  `src/xnglo_core/core.cpp` into a Visual Studio project (or CMake with
  MSVC) by hand, matching the Notepad++ plugin template's project
  settings (character set: Unicode, subsystem: Windows, exports the 6
  functions in `dllmain.cpp`).
- No real-Notepad++ manual testing (selection encoding edge cases,
  undo/redo behavior, menu icon, etc.) -- the UTF-8 codepage check in
  `transliterate_selection()` is a guess at a reasonable safety net, not
  something that's actually been exercised against real Scintilla
  behavior.
