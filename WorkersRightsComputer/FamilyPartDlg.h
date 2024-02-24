#pragma once
#include "MyDialogEx.h"


// CFamilyPartDlg dialog

class CFamilyPartDlg : public CMyDialogEx
{
	DECLARE_DYNAMIC(CFamilyPartDlg)

public:
	CFamilyPartDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFamilyPartDlg();
	virtual BOOL OnInitDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FAMILY_PART };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void UpdateText();
	void AddPeriod(bool bByHour);
	bool CheckInputValue(int id, double& oValue, double minVal, double maxVal, const wchar_t* zWhat);
	CDateTimeCtrl mStartPeriodGUI;

	// Information for next period - Company part
	CTime mStartTime;
	double mHoursPerWeek;
	double mPC;
	bool mbMinimum;
	double mHourlyWage;

	afx_msg void OnBnClickedButtonAddPeriod();
	afx_msg void OnBnClickedButtonFamilyPartClear();
	afx_msg void OnBnClickedButtonAddPeriodPercent();
	afx_msg void OnBnClickedButtonFamilyPartClearLast();
	void ClearForEdit();
	afx_msg void OnBnClickedRadioCompanyMinimum();
	afx_msg void OnBnClickedRadioCompanyHourly();
};
