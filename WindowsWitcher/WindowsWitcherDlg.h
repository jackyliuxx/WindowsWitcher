
// WindowsWitcherDlg.h : header file
//

#pragma once

// CWindowsWitcherDlg dialog
class CWindowsWitcherDlg : public CDialogEx
{
// Construction
public:
	CWindowsWitcherDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINDOWSWITCHER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CEdit m_MessageEdit;
	CListBox m_KeysList;
	CEdit m_WindowsEdit;

	struct Group {
		std::vector<CWnd*> windows;
		int counter = 0;
		ULONGLONG timer = 0;
	};
	std::map<int, Group> groups;

	bool ctrl_is_down;
	bool shift_is_down;
	int foreground_group;
	std::vector<CWnd*> foreground_group_windows;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	BOOL SetRawInput(HWND hWnd);
	VOID RawInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID HandleKeyInput(RAWKEYBOARD rawKB);
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	VOID PrintMessage(CString msg);
	BOOL IsAccepedWindow(CWnd* window);
};
