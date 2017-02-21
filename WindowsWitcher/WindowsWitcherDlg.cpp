
// WindowsWitcherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WindowsWitcher.h"
#include "WindowsWitcherDlg.h"
#include "afxdialogex.h"

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
	ctrl_is_down = false;
	shift_is_down = false;
	foreground_group = -1;
	for (int i = 0; i < 12; i++) {
		CString kn;
		kn.Format(L"F%d", i + 13);
		m_KeysList.AddString(kn);
	}
	m_KeysList.SetCurSel(0);
	SetTimer(0, 50, NULL);
	if (!SetRawInput(m_hWnd)) {
		AfxMessageBox(L"Fail to Rigister Raw Input");
		exit(0);
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

VOID CWindowsWitcherDlg::HandleKeyInput(RAWKEYBOARD rawKB) {
	auto vkCode = rawKB.VKey;
	auto scanCode = rawKB.MakeCode;
	auto flags = rawKB.Flags;
	auto message = rawKB.Message;
	bool down = flags % 2 == 0;

	if (scanCode == 29) {
		ctrl_is_down = down;
	}
	if (scanCode == 42 || scanCode == 54) {
		shift_is_down = down;
	}

	if (scanCode < 100 || scanCode >= 112) {
		return;
	}

	CString key_name;
	key_name.Format(L"F%d", scanCode - 87);
	CString msg;
	auto &group = groups[scanCode];
	auto &windows = group.windows;
	if (ctrl_is_down || shift_is_down) {
		if (down) {
			if (ctrl_is_down) {
				windows.clear();
				msg.Format(L"Clear\t%s\r\n", key_name);
				PrintMessage(msg);
			}
			if (shift_is_down) {
				auto w = GetForegroundWindow();
				if (IsAccepedWindow(w)) {
					CString tmp;
					w->GetWindowTextW(tmp);
					if (std::find(windows.begin(), windows.end(), w) == windows.end()) {
						windows.push_back(w);
						msg.Format(L"Push\t%s : %s\r\n", key_name, tmp);
					}
					else {
						msg.Format(L"Exists\t%s : %s\r\n", key_name, tmp);
					}
					PrintMessage(msg);
				}
			}
		}
	}
	else {
		if (down) {
			if (group.counter == 0) {
				if (foreground_group == scanCode) {
					for (auto w : foreground_group_windows) {
						w->ShowWindow(SW_MINIMIZE);
					}
					foreground_group = -1;
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
				msg.Format(L"%s down\r\n", key_name);
				PrintMessage(msg);
			}
		}
		else {
			if (group.counter == 1) {
				for (int i = windows.size() - 1; i >= 0; i--) {
					auto w = windows[i];
					w->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				}
				if (GetTickCount64() - group.timer < 500) {
					foreground_group_windows.clear();
					for (int i = windows.size() - 1; i >= 0; i--) {
						auto w = windows[i];
						if (i == 0) {
							while (GetForegroundWindow() != w) {
								w->SetForegroundWindow();
							}
						}
						foreground_group_windows.push_back(w);
					}
					foreground_group = scanCode;
					msg.Format(L"%s Up\r\n", key_name);
				}
				else {
					if (foreground_group != -1) {
						auto &fgw = foreground_group_windows;
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
					msg.Format(L"%s Long Up\r\n", key_name);
				}
			}
			group.counter = 0;
			PrintMessage(msg);
		}
	}
}

BOOL CWindowsWitcherDlg::SetRawInput(HWND hWnd) {
	RAWINPUTDEVICE Rid;
	Rid.usUsagePage = 0x01;
	Rid.usUsage = 0x06;
	Rid.dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK;
	Rid.hwndTarget = hWnd;
	return RegisterRawInputDevices(&Rid, 1, sizeof(Rid));
}

VOID CWindowsWitcherDlg::RawInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	UINT dwSize;

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	LPBYTE lpb = new BYTE[dwSize];
	if (lpb == NULL) {
		return;
	}

	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize,
		sizeof(RAWINPUTHEADER)) != dwSize)
		OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

	RAWINPUT* raw = (RAWINPUT*)lpb;
	HandleKeyInput(raw->data.keyboard);
	delete[] lpb;
}

BOOL CWindowsWitcherDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_INPUT)
		RawInput(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
	return CDialog::PreTranslateMessage(pMsg);
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
		if (IsAccepedWindow(fw) && foreground_group != -1 && !foreground_group_windows.empty()) {
			auto &fgw = foreground_group_windows;
			for (int i = 0; i < (int) fgw.size() - 1; i++) {
				if (fgw[i] == fw) {
					std::swap(fgw[i], fgw[i + 1]);
				}
			}
			if (fgw.back() != fw) {
				foreground_group = -1;
				foreground_group_windows.resize(1);
			}
		}

		CString cur_text;
		CString new_text;
		m_WindowsEdit.GetWindowTextW(cur_text);
		int key = m_KeysList.GetCurSel() + 100;
		for (int i = 0; i < (int) groups[key].windows.size(); i++) {
			CString title;
			groups[key].windows[i]->GetWindowTextW(title);
			new_text.Append(title + L"\r\n");
		}
		if (cur_text != new_text) {
			m_WindowsEdit.SetWindowTextW(new_text);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

VOID CWindowsWitcherDlg::PrintMessage(CString msg) {
	CString str;
	m_MessageEdit.GetWindowTextW(str);
	str.Append(msg);
	m_MessageEdit.SetWindowTextW(str);
	m_MessageEdit.SetSel(0xFFFFFFF, 0xFFFFFFF);
}

BOOL CWindowsWitcherDlg::IsAccepedWindow(CWnd* window) {
	BOOL rv = window && IsWindow(window->m_hWnd) && window->GetWindowTextLengthW() > 0;
	return rv;
}