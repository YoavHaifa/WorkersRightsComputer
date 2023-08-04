#pragma once
#include "MyTime.h"
#include "MyDialogEx.h"

// CVacationsDlg dialog

class CVacationsDlg : public CMyDialogEx
{
	DECLARE_DYNAMIC(CVacationsDlg)

public:
	CVacationsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CVacationsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VACATIONS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	void SetMaternityLeave(bool flag);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonVacAdd();
	CMyTime mStart;
	CMyTime mEnd;
	double mnCurrentWeeks;
	CDateTimeCtrl mEditFirstDay;
	CDateTimeCtrl mEditLastDay;
	void UpdateText();
	afx_msg void OnBnClickedButtonVacClearLast();
	afx_msg void OnBnClickedButtonVacClear();
	afx_msg void OnBnClickedCheck14daysVac4sevrance();
	CButton mIsMaternityLeave;
	CEdit mnPaidMLWeeks;
	CButton mDeservesPension;
	afx_msg void OnBnClickedCheckMaternity();
};

