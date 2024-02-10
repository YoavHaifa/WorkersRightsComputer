#pragma once
#include "MyDialogEx.h"


// CLetterSavedDlg dialog

class CLetterSavedDlg : public CMyDialogEx
{
	DECLARE_DYNAMIC(CLetterSavedDlg)

public:
	CLetterSavedDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLetterSavedDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LETTER_SAVED };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString msFileName;
	void SetFileName(CString& sfName);
};
