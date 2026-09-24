// WINDOWS-ONLY. Cannot be compiled or tested in this repo's Linux
// sandbox -- see ../../CLAUDE.md.
//
// Required exports for any Notepad++ plugin DLL: isUnicode, getName,
// setInfo, beNotified, messageProc, getFuncsArray (see the official
// plugin template for the full contract this mirrors).
#include "PluginDefinition.h"

BOOL APIENTRY DllMain(HANDLE /*hModule*/, DWORD reasonForCall, LPVOID /*lpReserved*/) {
  switch (reasonForCall) {
    case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      break;
  }
  return TRUE;
}

extern "C" __declspec(dllexport) void setInfo(NppData notepadPlusData) {
  setNppData(notepadPlusData);
}

extern "C" __declspec(dllexport) const TCHAR* getName() {
  return TEXT("htr-xnglo");
}

extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int* nbF) {
  return ::getFuncsArray(nbF);
}

extern "C" __declspec(dllexport) void beNotified(SCNotification* /*notifyCode*/) {
  // No notifications handled yet (nothing needs NPPN_SHUTDOWN etc. so far
  // -- pluginCleanUp() is a no-op currently).
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT /*Message*/, WPARAM /*wParam*/, LPARAM /*lParam*/) {
  return TRUE;
}

extern "C" __declspec(dllexport) BOOL isUnicode() {
  return TRUE;
}
