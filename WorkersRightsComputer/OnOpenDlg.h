#pragma once
#include "MyDialogEx.h"


// COnOpenDlg dialog

class COnOpenDlg : public CMyDialogEx
{
	DECLARE_DYNAMIC(COnOpenDlg)

public:
	COnOpenDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~COnOpenDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ON_OPEN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDefineDir();
	void UpdateFromGUI();
	bool VerifySaveDir();
	bool mbDefineDirCanceled;
};
