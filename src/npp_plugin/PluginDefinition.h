// WINDOWS-ONLY. Cannot be compiled or tested in this repo's Linux
// sandbox -- see ../../CLAUDE.md.
#pragma once

#ifndef _WIN32
#error "PluginDefinition.h is Windows-only (Win32 API + Notepad++ SDK)."
#endif

#include "PluginInterface.h"

// Called from dllmain.cpp's NppData setter.
void setNppData(NppData notepadPlusData);

// Called from dllmain.cpp's getFuncsArray export.
FuncItem* getFuncsArray(int* nbF);

// Menu command callbacks (registered in getFuncsArray's table in
// PluginDefinition.cpp). Each replaces the current Scintilla selection
// with the transliterated text.
void menu_transliterate_xi38();
void menu_transliterate_u38();

void pluginCleanUp();
