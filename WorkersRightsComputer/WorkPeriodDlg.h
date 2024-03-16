#pragma once
//#include "MyDialogEx.h"
#include "WageDefBaseDlg.h"

// CWorkPeriodDlg dialog

class CWorkPeriodDlg : public CWageDefBaseDlg
{
	DECLARE_DYNAMIC(CWorkPeriodDlg)

public:
	CWorkPeriodDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CWorkPeriodDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WORK_PERIOD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	void SetWageGui();

	DECLARE_MESSAGE_MAP()
public:
	CDateTimeCtrl mStartDate;
	CDateTimeCtrl mEndDate;
	CDateTimeCtrl mNoticeDate;
	afx_msg void OnDtnDatetimechangeDatetimepickerStart(NMHDR *pNMHDR, LRESULT *pResult);
	void UpdateText();
	CEdit mShow;
	afx_msg void OnDtnDatetimechangeDatetimepickerEnd(NMHDR *pNMHDR, LRESULT *pResult);
	bool mbStartSet;
	bool mbEndSet;
	bool mbNoticeSet;
	bool mbDialogInitialized;
	afx_msg void OnDtnDatetimechangeDatetimepickerNotice(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit mMonthlySalary;
	CEdit mHourlySalary;
	CEdit mHoursPerWeek;
	afx_msg void OnBnClickedRadioMinWage();
	afx_msg void OnBnClickedRadioMonthly();
	afx_msg void OnBnClickedRadioHourly();
	void SetWageMode(int mode);
	//CString GetWageText();
	afx_msg void OnEnChangeEditMonthSalary();
	afx_msg void OnEnChangeEditHourSalary();
	afx_msg void OnEnChangeEditHoursPerMonth();
	afx_msg void OnBnClickedOk();
	bool UpdateDataFromDialog(void);
	bool SetWageForWholePeriod();

	// Radio buttons for working days
	CButton mCheckSunday;
	CButton mCheckMonday;
	CButton mCheckTuesday;
	CButton mCheckWednesday;
	CButton mCheckThursday;
	CButton mCheckFriday;
	CButton mCheckSaturday;
	CButton *mapCheckDays[7];

	// Radio buttons for wage definition mode
	CButton mRadioMinWage;
	CButton mRadioMonthly;
	CButton mRadioHourly;
	CButton mRadioWagePeriods;

	CString GetDaysText();
	afx_msg void OnBnClickedCheckSunday();
	afx_msg void OnBnClickedCheckMonday();
	afx_msg void OnBnClickedCheckTuesday();
	afx_msg void OnBnClickedCheckWednesday();
	afx_msg void OnBnClickedCheckThursday();
	afx_msg void OnBnClickedCheckFriday();
	afx_msg void OnBnClickedCheckSaturday();
	afx_msg void OnBnClickedOk3();
	afx_msg void OnBnClickedButtonFamilyPart();
	//CDateTimeCtrl mLastSalaryDate;
	afx_msg void OnBnClickedCheckNoNotice();
	afx_msg void OnBnClickedButtonEditWage();
	afx_msg void OnBnClickedRadioDiffWages();
	afx_msg void OnBnClickedButtonSetWage();
	afx_msg void OnBnClickedCheckMonthlyBonus();
};
