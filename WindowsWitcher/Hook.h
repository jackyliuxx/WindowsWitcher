#pragma once

#define WM_KEYINPUT (WM_USER + 102)

BOOL SetLowLevelKeyboardHook(BOOL bInstall, HWND hWndCaller = NULL);