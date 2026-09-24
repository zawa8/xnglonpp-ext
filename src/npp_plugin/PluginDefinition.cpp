// WINDOWS-ONLY. Cannot be compiled or tested in this repo's Linux
// sandbox -- see ../../CLAUDE.md for what has and hasn't been verified.
#include "PluginDefinition.h"
#include "../xnglo_core/core.h"

#include <vector>

namespace {

NppData g_nppData;
FuncItem g_funcItems[2];

// Scintilla message constants this plugin needs (subset -- the full set
// lives in Scintilla.h, shipped with the Notepad++ plugin template; only
// what's used here is duplicated to keep this file self-contained).
constexpr int SCI_GETSELECTIONSTART = 2143;
constexpr int SCI_GETSELECTIONEND = 2145;
constexpr int SCI_GETTEXTRANGE = 2162;
constexpr int SCI_REPLACESEL = 2170;
constexpr int SCI_GETCODEPAGE = 2137;
constexpr int SC_CP_UTF8 = 65001;

struct Sci_TextRange {
  struct { long cpMin; long cpMax; } chrg;
  char* lpstrText;
};

HWND current_scintilla() {
  int which = -1;
  ::SendMessage(g_nppData._nppHandle, /*NPPM_GETCURRENTSCINTILLA*/ 2000 + 4, 0,
                reinterpret_cast<LPARAM>(&which));
  return which == 0 ? g_nppData._scintillaMainHandle : g_nppData._scintillaSecondHandle;
}

// Reads the current selection as UTF-8, runs `transform` over it, and
// writes the result back, replacing the selection. Notepad++ documents
// (Scintilla buffers) aren't guaranteed UTF-8 -- if the buffer's code
// page isn't SC_CP_UTF8 this bails out rather than mangling non-UTF-8
// bytes as if they were, since xnglo_core::to_xi38/to_u38 assume UTF-8
// input.
void transliterate_selection(std::string (*transform)(const std::string&)) {
  HWND sci = current_scintilla();
  if (::SendMessage(sci, SCI_GETCODEPAGE, 0, 0) != SC_CP_UTF8) {
    ::MessageBox(g_nppData._nppHandle,
                 TEXT("This document's encoding isn't UTF-8. Switch it to UTF-8 ")
                 TEXT("(Encoding menu) before transliterating, or the selected text ")
                 TEXT("may come through wrong."),
                 TEXT("htr-xnglo"), MB_OK | MB_ICONWARNING);
    return;
  }

  int start = static_cast<int>(::SendMessage(sci, SCI_GETSELECTIONSTART, 0, 0));
  int end = static_cast<int>(::SendMessage(sci, SCI_GETSELECTIONEND, 0, 0));
  if (start == end) return; // nothing selected

  std::vector<char> buf(static_cast<size_t>(end - start) + 1, 0);
  Sci_TextRange tr;
  tr.chrg.cpMin = start;
  tr.chrg.cpMax = end;
  tr.lpstrText = buf.data();
  ::SendMessage(sci, SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));

  std::string selected(buf.data());
  std::string result = transform(selected);
  ::SendMessage(sci, SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(result.c_str()));
}

}  // namespace

void setNppData(NppData notepadPlusData) { g_nppData = notepadPlusData; }

void menu_transliterate_xi38() { transliterate_selection(xnglo::to_xi38); }
void menu_transliterate_u38() { transliterate_selection(xnglo::to_u38); }

FuncItem* getFuncsArray(int* nbF) {
  lstrcpy(g_funcItems[0]._itemName, TEXT("Transliterate selection -> xi38 (full romanization)"));
  g_funcItems[0]._pFunc = menu_transliterate_xi38;
  g_funcItems[0]._init2Check = false;
  g_funcItems[0]._pShKey = nullptr;

  lstrcpy(g_funcItems[1]._itemName, TEXT("Transliterate selection -> u38 (keep native letters)"));
  g_funcItems[1]._pFunc = menu_transliterate_u38;
  g_funcItems[1]._init2Check = false;
  g_funcItems[1]._pShKey = nullptr;

  *nbF = 2;
  return g_funcItems;
}

void pluginCleanUp() {}
