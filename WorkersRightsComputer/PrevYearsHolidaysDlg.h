#pragma once
#include "MyDialogEx.h"


// CPrevYearsHolidaysDlg dialog

class CPrevYearsHolidaysDlg : public CMyDialogEx
{
	DECLARE_DYNAMIC(CPrevYearsHolidaysDlg)

public:
	CPrevYearsHolidaysDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPrevYearsHolidaysDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PREV_HOLIDAYS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	bool mbInitializing;

	afx_msg void OnEnChangeEditHolidaysPrevy();
	afx_msg void OnEnChangeEditHolidaysPrevy1();
	afx_msg void OnEnChangeEditHolidaysPrevy2();
	afx_msg void OnEnChangeEditHolidaysPrevy3();
	afx_msg void OnEnChangeEditHolidaysPrevy4();
	afx_msg void OnEnChangeEditHolidaysPrevy5();
	afx_msg void OnEnChangeEditHolidaysPrevy6();
	afx_msg void OnEnChangeEditHolidaysPrevy7();
};
