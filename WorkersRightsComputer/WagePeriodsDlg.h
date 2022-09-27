#pragma once
#include "MyDialogEx.h"
#include "MyTime.h"
#include "WagePeriod.h"


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

	// Data for new period
	CMyTime mFirst;
	CMyTime mLast;
	EWageMode meMode; // Must be "MIN", "MONTHLY" or "HOURLY"
	double mMonthly;
	double mHourly;
	double mnHours;

	DECLARE_MESSAGE_MAP()

	void UpdateState();
	CDateTimeCtrl mStartPeriod;
	CDateTimeCtrl mLastInPeriod;
	afx_msg void OnBnClickedButtonAddWagePeriod();

	bool GetInputTime();
	bool GetWage();
};
