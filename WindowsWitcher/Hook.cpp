#include "stdafx.h"
#include "Hook.h"

HWND g_hWnd = NULL;	        // Window handle
HHOOK g_hHook = NULL;		// Hook handle

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0 || nCode == HC_NOREMOVE)
		return CallNextHookEx(g_hHook, nCode, wParam, lParam);

	if (lParam & 0x40000000)	// Check the previous key state
	{
		return CallNextHookEx(g_hHook, nCode, wParam, lParam);
	}

	KBDLLHOOKSTRUCT  *pkbhs = (KBDLLHOOKSTRUCT *)lParam;

	//check that the message is from keyboard or is synthesized by SendInput API
	if ((pkbhs->flags & LLKHF_INJECTED))
		return CallNextHookEx(g_hHook, nCode, wParam, lParam);

	// Post private messages to Main window
	LPARAM lp = (pkbhs->flags << 24) + (pkbhs->scanCode << 16);
	PostMessage(g_hWnd, WM_KEYINPUT, pkbhs->vkCode, lp);

	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

BOOL SetLowLevelKeyboardHook(BOOL bInstall, HWND hWndCaller)
{
	BOOL bOk;
	g_hWnd = hWndCaller;

	if (bInstall)
	{
		g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL,
					LowLevelKeyboardProc, NULL, NULL);
		bOk = (g_hHook != NULL);
	}
	else
	{
		bOk = UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}

	return bOk;
}