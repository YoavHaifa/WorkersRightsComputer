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

	static CWagePeriodsDlg* umpDlg;
	void UpdateState(const char *zTitle = NULL);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WAGE_PERIODS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	void SetTimes();

	CEdit mShowPeriods;

	// Data for new period
	CMyTime mFirst;
	CMyTime mLast;
	EWageMode meMode; // Must be "MIN", "MONTHLY" or "HOURLY"
	double mMonthly;
	double mHourly;
	double mnHoursPerMonth;

	CString msState;

	DECLARE_MESSAGE_MAP()

	CDateTimeCtrl mStartPeriod;
	CDateTimeCtrl mLastInPeriod;
	afx_msg void OnBnClickedButtonAddWagePeriod();
	virtual void OnOK();

	void ClearNumericFields();

	bool GetInputTime();
	bool GetWage();
public:
	afx_msg void OnBnClickedRadioMinWage();
	afx_msg void OnBnClickedRadioMonthly();
	afx_msg void OnBnClickedRadioHourly();
};
