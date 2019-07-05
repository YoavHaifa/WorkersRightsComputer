#pragma once
#include "MyDialogEx.h"


// CMyAskDlg dialog

class CMyAskDlg : public CMyDialogEx
{
	DECLARE_DYNAMIC(CMyAskDlg)

public:
	CMyAskDlg(const wchar_t *zTitle, const wchar_t *zText, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMyAskDlg();

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	bool Ask(void);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CString msTitle;
	CString msText;
	bool mbOK;
};
