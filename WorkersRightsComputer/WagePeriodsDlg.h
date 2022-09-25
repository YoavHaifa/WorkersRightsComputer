#pragma once
#include "MyDialogEx.h"


// CWagePeriods dialog

class CWagePeriodsDlg : public CMyDialogEx
{
	DECLARE_DYNAMIC(CWagePeriodsDlg)

public:
	CWagePeriodsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CWagePeriodsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WAGE_PERIODS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	CEdit mShowPeriods;

	DECLARE_MESSAGE_MAP()
public:
	void UpdateState();
};
