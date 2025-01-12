#pragma once
#include "MyDialogEx.h"

// CRealHolidaysDlg dialog

class CRealHolidaysDlg : public CMyDialogEx
{
	DECLARE_DYNAMIC(CRealHolidaysDlg)

public:
	CRealHolidaysDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CRealHolidaysDlg();

	bool UpdateDaysDataFromDialog(void);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REAL_HOLIDAYS };
#endif

protected:
	static const int N_WEEK_DAYS = 7;

	friend class CHolidaysByDay;

	// Radio buttons for working days
	CButton mCheckSunday;
	CButton mCheckMonday;
	CButton mCheckTuesday;
	CButton mCheckWednesday;
	CButton mCheckThursday;
	CButton mCheckFriday;
	CButton mCheckSaturday;
	CButton* mapCheckDays[N_WEEK_DAYS];

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	CComboBox mComboHolidays;
	void InitHolidaysCombo();
	afx_msg void OnCbnSelchangeComboHolidays();

	afx_msg void OnBnClickedCheckSunday();
	afx_msg void OnBnClickedCheckMonday();
	afx_msg void OnBnClickedCheckTuesday();
	afx_msg void OnBnClickedCheckWednesday();
	afx_msg void OnBnClickedCheckThursday();
	afx_msg void OnBnClickedCheckFriday();
	afx_msg void OnBnClickedCheckSaturday();

	DECLARE_MESSAGE_MAP()
};
