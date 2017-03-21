
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
	std::map<LONG, Group> groups;

	bool ctrlDown;
	bool shiftDown;
	LONG foregroundGroup;
	std::vector<CWnd*> foregroundGroupWindows;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnKeyInput(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	template<typename... T>
	VOID PrintMessage(const CString &format, T&&... args);
	BOOL IsAccepedWindow(CWnd* window);
	DWORD GetKeyNameCode(LPARAM lParam);
	CString GetKeyName(LONG keyNameCode);
	CString GetCustomKeyName(LONG keyNameCode);
};
