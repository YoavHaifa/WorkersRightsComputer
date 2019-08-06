#pragma once
#include "MyDialogEx.h"


// CFamilyPartDlg dialog

class CFamilyPartDlg : public CMyDialogEx
{
	DECLARE_DYNAMIC(CFamilyPartDlg)

public:
	CFamilyPartDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFamilyPartDlg();
	virtual BOOL OnInitDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FAMILY_PART };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void UpdateText();
	CDateTimeCtrl mStartPeriod;
	afx_msg void OnBnClickedButtonAddPeriod();
	afx_msg void OnBnClickedButtonFamilyPartClear();
	afx_msg void OnBnClickedButtonAddPeriodPercent();
	afx_msg void OnBnClickedButtonFamilyPartClearLast();
	void ClearForEdit();
};
