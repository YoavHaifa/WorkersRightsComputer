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

	void SetMaternityLeave();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonVacAdd();
	afx_msg void OnBnClickedButtonVacClearLast();
	afx_msg void OnBnClickedButtonVacClear();
	afx_msg void OnBnClickedCheck14daysVac4sevrance();
	afx_msg void OnBnClickedCheckMaternity();
	afx_msg void OnDtnDatetimechangeDatetimepickerVacStart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDtnDatetimechangeDatetimepickerVacEnd(NMHDR* pNMHDR, LRESULT* pResult);

	void UpdateText();
	void UpdateDisplayedLength();

	CMyTime mStart;
	CMyTime mEnd;
	bool mbStartSet;
	bool mbEndSet;
	bool mbMaternity;
	double mnCurrentWeeks;
	CDateTimeCtrl mEditFirstDay;
	CDateTimeCtrl mEditLastDay;
	CButton mIsMaternityLeave;
	CEdit mnPaidMLWeeks;
	CButton mDeservesPension;
};

