#pragma once
#include "MyDialogEx.h"


// CEmployerDlg dialog

class CEmployerDlg : public CMyDialogEx
{
	DECLARE_DYNAMIC(CEmployerDlg)

public:
	CEmployerDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CEmployerDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EMPLOYER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAddEmployer();
	void AddContact(bool bEmployer);
	bool Set(int id, CString& s);
	void UpdateList();
	void ClearEdit();
	afx_msg void OnBnClickedButtonClearContacts();
	CEdit mCommentEdit;
	CEdit mListDesc;
};
