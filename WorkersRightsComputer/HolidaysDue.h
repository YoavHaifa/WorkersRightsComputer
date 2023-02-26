#pragma once
#include "afxtempl.h"
#include "HolidaysDuePerYear.h"
#include "MyTime.h"

class CHolidaysDue
{
public:
	CHolidaysDue();

	void Reset();
	bool VerifyWorkPeriod(class CMyDialogEx* pMainDlg);
	void SetSavedWorkPeriod();
	void SetWorkPeriod();
	void InitDialog(CMyDialogEx* pDlg);
	void OnGuiChange(int iYear, CMyDialogEx* pDlg);
	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pWorkPeriod, CXMLParseNode* pRoot);

	// Support Main WRC Dialog
	void UpdateMainDialog();
	void OnMainDialogChange(CMyDialogEx* pMainDlg);

	// Interface for computations
	int GetNDueLastYear();
	int GetNPrevYears();
	int GetNDuePrevYear(int i);

private:
	void LoadFromOldXml(CXMLParseNode* pRoot);
	void SetYearsByWorkPeriod();

	CMyTime mFirstInPeriod;
	CMyTime mLastInPeriod;
	CString msHolidaysSelection;

	bool mbPeriodAndHolidaysDefined;
	bool mbDefinedBySpecialDialog;

	void UpdateSum(CMyDialogEx* pDlg = NULL);

	int mn;
	CList <class CHolidaysDuePerYear*, CHolidaysDuePerYear*> mHolidaysPerYer;
	CHolidaysDuePerYear mSum;
	CHolidaysDuePerYear mSumPrev;
	CHolidaysDuePerYear* mpLastYear;

	static const int N_MAIN_DLG_FIELDS = 11;
	unsigned int maMainDlgFields[N_MAIN_DLG_FIELDS];
};

extern CHolidaysDue gHolidaysDue;

