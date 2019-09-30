#pragma once
#include "MyDialogEx.h"


// CWorkPeriodDlg dialog

class CWorkPeriodDlg : public CMyDialogEx
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

	DECLARE_MESSAGE_MAP()
public:
	CDateTimeCtrl mStartDate;
	CDateTimeCtrl mEndDate;
	CDateTimeCtrl mNoticeDate;
	afx_msg void OnDtnDatetimechangeDatetimepickerStart(NMHDR *pNMHDR, LRESULT *pResult);
	int UpdateText();
	CEdit mShow;
	afx_msg void OnDtnDatetimechangeDatetimepickerEnd(NMHDR *pNMHDR, LRESULT *pResult);
	bool mbStartSet;
	bool mbEndSet;
	bool mbNoticeSet;
	afx_msg void OnDtnDatetimechangeDatetimepickerNotice(NMHDR *pNMHDR, LRESULT *pResult);
	CButton mRadioMinWage;
	CEdit mMonthlySalary;
	CEdit mHourlySalary;
	CEdit mHoursPerWeek;
	afx_msg void OnBnClickedRadioMinWage();
	afx_msg void OnBnClickedRadioMonthly();
	afx_msg void OnBnClickedRadioHourly();
	void SetWageMode(int mode);
	int mWageMode;
	CString GetWageText();
	afx_msg void OnEnChangeEditMonthSalary();
	afx_msg void OnEnChangeEditHourSalary();
	afx_msg void OnEnChangeEditHoursPerWeek();
	afx_msg void OnBnClickedOk();
	void UpdateDataFromDialog(void);
	CButton mCheckSunday;
	CButton mCheckMonday;
	CButton mCheckTuesday;
	CButton mCheckWednesday;
	CButton mCheckThursday;
	CButton mCheckFriday;
	CButton mCheckSaturday;
	CButton *mapCheckDays[7];
	CButton mRadioMonthly;
	CButton mRadioHourly;
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
};
