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

	DECLARE_MESSAGE_MAP()
};
