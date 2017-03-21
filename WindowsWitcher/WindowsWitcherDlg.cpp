
// WindowsWitcherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WindowsWitcher.h"
#include "WindowsWitcherDlg.h"
#include "afxdialogex.h"

#include "Hook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWindowsWitcherDlg dialog



CWindowsWitcherDlg::CWindowsWitcherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WINDOWSWITCHER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWindowsWitcherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESSAGE_EDIT, m_MessageEdit);
	DDX_Control(pDX, IDC_KEYS_LIST, m_KeysList);
	DDX_Control(pDX, IDC_WINDOWS_EDIT, m_WindowsEdit);
}

BEGIN_MESSAGE_MAP(CWindowsWitcherDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_MESSAGE(WM_KEYINPUT, OnKeyInput)
END_MESSAGE_MAP()


// CWindowsWitcherDlg message handlers

BOOL CWindowsWitcherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	ctrlDown = false;
	shiftDown = false;
	foregroundGroup = -1;

	groups[0x4F0000]; // num 1
	groups[0x500000]; // num 2
	groups[0x510000]; // num 3

	for (auto &g : groups) {
		m_KeysList.AddString(GetCustomKeyName(g.first));
	}
	m_KeysList.SetCurSel(0);
	SetTimer(0, 50, NULL);

	if (!SetLowLevelKeyboardHook(TRUE, m_hWnd))
	{
		PrintMessage(L"Fail to Set Hook!");
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWindowsWitcherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWindowsWitcherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWindowsWitcherDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0) {
		for (auto &gp : groups) {
			auto &windows = gp.second.windows;
			for (int i = 0; i < (int) windows.size(); i++) {
				if (!IsWindow(windows[i]->m_hWnd)) {
					windows.erase(windows.begin() + i);
					i--;
				}
			}
		}

		auto fw = GetForegroundWindow();
		if (IsAccepedWindow(fw) && foregroundGroup != -1 && !foregroundGroupWindows.empty()) {
			auto &fgw = foregroundGroupWindows;
			for (int i = 0; i < (int) fgw.size() - 1; i++) {
				if (fgw[i] == fw) {
					std::swap(fgw[i], fgw[i + 1]);
				}
			}
			if (fgw.back() != fw) {
				foregroundGroup = -1;
				foregroundGroupWindows.resize(1);
			}
		}

		CString cur_text;
		CString new_text;
		m_WindowsEdit.GetWindowText(cur_text);
		auto it = groups.begin();
		std::advance(it, m_KeysList.GetCurSel());
		int key = it->first;
		for (int i = 0; i < (int) groups[key].windows.size(); i++) {
			CString title;
			groups[key].windows[i]->GetWindowText(title);
			new_text.Append(title + L"\r\n");
		}
		if (cur_text != new_text) {
			m_WindowsEdit.SetWindowText(new_text);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

LRESULT CWindowsWitcherDlg::OnKeyInput(WPARAM wParam, LPARAM lParam) {
	DWORD flags = lParam >> 24;
	DWORD scanCode = (lParam >> 16) & 0xFF;
	DWORD keyNameCode = GetKeyNameCode(lParam);
	CString keyName = GetKeyName(keyNameCode);
	BOOL keyDown = !(flags & KF_UP);
	//PrintMessage(L"%s\t| keyNameCode: %x\t| %s\r\n",
	//	keyDown ? L"Down" : L"Up", keyNameCode, keyName);

	// Ctrl
	if (scanCode == 29) {
		ctrlDown = keyDown;
		return 0;
	}
	// Shift
	if (scanCode == 42 || scanCode == 54) {
		shiftDown = keyDown;
		return 0;
	}
	if (groups.find(keyNameCode) == groups.end()) {
		return 0;
	}

	auto &group = groups[keyNameCode];
	auto &windows = group.windows;
	if (ctrlDown || shiftDown) {
		if (keyDown) {
			if (ctrlDown) {
				windows.clear();
				PrintMessage(L"Clear\t%s\r\n", keyName);
			}
			if (shiftDown) {
				auto w = GetForegroundWindow();
				if (IsAccepedWindow(w)) {
					CString tmp;
					w->GetWindowText(tmp);
					if (std::find(windows.begin(), windows.end(), w) == windows.end()) {
						windows.push_back(w);
						PrintMessage(L"Push\t%s : %s\r\n", keyName, tmp);
					}
					else {
						PrintMessage(L"Exists\t%s : %s\r\n", keyName, tmp);
					}
				}
			}
		}
	}
	else {
		if (keyDown) {
			if (group.counter == 0) {
				if (foregroundGroup == keyNameCode) {
					for (auto w : foregroundGroupWindows) {
						w->ShowWindow(SW_MINIMIZE);
					}
					foregroundGroup = -1;
					group.counter = 3;
				}
				else {
					for (int i = windows.size() - 1; i >= 0; i--) {
						auto w = windows[i];
						if (w->IsIconic()) {
							auto fw = GetForegroundWindow();
							w->ShowWindow(SW_SHOWNOACTIVATE);
							while (fw && GetForegroundWindow() != fw) {
								fw->SetForegroundWindow();
							}
						}
						w->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
						w->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
					}
					group.counter = 1;
					group.timer = GetTickCount64();
				}
				PrintMessage(L"%s keyDown\r\n", keyName);
			}
		}
		else {
			if (group.counter == 1) {
				for (int i = windows.size() - 1; i >= 0; i--) {
					auto w = windows[i];
					w->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				}
				if (GetTickCount64() - group.timer < 500) {
					foregroundGroupWindows.clear();
					for (int i = windows.size() - 1; i >= 0; i--) {
						auto w = windows[i];
						if (i == 0) {
							while (GetForegroundWindow() != w) {
								w->SetForegroundWindow();
							}
						}
						foregroundGroupWindows.push_back(w);
					}
					foregroundGroup = keyNameCode;
					PrintMessage(L"%s Up\r\n", keyName);
				}
				else {
					if (foregroundGroup != -1) {
						auto &fgw = foregroundGroupWindows;
						for (auto w : fgw) {
							w->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
							w->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
						}
					}
					else {
						auto fw = GetForegroundWindow();
						if (IsAccepedWindow(fw)) {
							fw->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
							fw->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
						}
					}
					PrintMessage(L"%s Long Up\r\n", keyName);
				}
			}
			group.counter = 0;
		}
	}
	return 0;
}

template<typename... T>
VOID CWindowsWitcherDlg::PrintMessage(const CString &format, T&&... args) {
	CString msg;
	msg.Format(format, args...);
	CString str;
	m_MessageEdit.GetWindowText(str);
	str.Append(msg);
	m_MessageEdit.SetWindowText(str);
	m_MessageEdit.SetSel(0xFFFFFFF, 0xFFFFFFF);
}

BOOL CWindowsWitcherDlg::IsAccepedWindow(CWnd* window) {
	BOOL rv = window && IsWindow(window->m_hWnd) && window->GetWindowTextLength() > 0;
	return rv;
}

DWORD CWindowsWitcherDlg::GetKeyNameCode(LPARAM lParam) {
	LONG code = lParam & 0x01FF0000;
	if (code == 0x01360000) // right shift
		code = 0x00360000;
	return code;
}

CString CWindowsWitcherDlg::GetKeyName(LONG keyNameCode) {
	CString keyName;
	GetKeyNameText(keyNameCode, keyName.GetBuffer(30), 30);
	keyName.ReleaseBuffer();
	return keyName;
}

CString CWindowsWitcherDlg::GetCustomKeyName(LONG keyNameCode) {
	return GetKeyName(keyNameCode);
}
