#pragma once
#include "MyDialogEx.h"


// CCommentsDlg dialog

class CCommentsDlg : public CMyDialogEx
{
	DECLARE_DYNAMIC(CCommentsDlg)

public:
	CCommentsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CCommentsDlg();

	virtual BOOL OnInitDialog();
	virtual void OnOK();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_COMMENTS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonClearComments();
};
