// Trimmed copy of the relevant parts of Notepad++'s official plugin
// template PluginInterface.h (https://github.com/npp-plugins/plugintemplate,
// itself derived from Notepad++'s own Notepad_plus_msgs.h /
// PluginInterface.h, distributed with every NPP plugin). Only what this
// plugin actually needs is kept here -- get the full official header from
// the plugin template repo if you need more of the API surface.
//
// WINDOWS-ONLY. Cannot be compiled or tested in this repo's Linux
// sandbox -- see ../../CLAUDE.md.
#pragma once

#ifndef _WIN32
#error "PluginInterface.h is Windows-only (Win32 API + Notepad++ SDK)."
#endif

#include <windows.h>

const int nbChar = 64;

struct NppData {
  HWND _nppHandle;
  HWND _scintillaMainHandle;
  HWND _scintillaSecondHandle;
};

struct FuncItem {
  TCHAR _itemName[nbChar];
  void (*_pFunc)();
  int _cmdID;
  bool _init2Check;
  ShortcutKey* _pShKey;
};

typedef const TCHAR* (__cdecl* PFUNCPLUGINCMD)();
typedef void (__cdecl* PFUNCSETINFO)(NppData);
typedef FuncItem* (__cdecl* PFUNCGETFUNCSARRAY)(int*);
typedef void (__cdecl* PBENOTIFIED)(SCNotification*);
typedef LRESULT (__cdecl* PMESSAGEPROC)(UINT, WPARAM, LPARAM);
