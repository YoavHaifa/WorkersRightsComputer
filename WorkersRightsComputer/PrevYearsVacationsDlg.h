#pragma once
#include "MyDialogEx.h"


// CPrevYearsVacationsDlg dialog

class CPrevYearsVacationsDlg : public CMyDialogEx
{
	DECLARE_DYNAMIC(CPrevYearsVacationsDlg)

public:
	CPrevYearsVacationsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPrevYearsVacationsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PREV_YEARS_VACATIONS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	bool mbInitializing;
};
