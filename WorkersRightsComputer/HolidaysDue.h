#pragma once
#include "afxtempl.h"
#include "HolidaysDuePerYear.h"
#include "MyTime.h"

class CHolidaysDue
{
public:
	CHolidaysDue();

	void Reset();
	void VerifyWorkPeriod();
	void SetSavedWorkPeriod();
	void SetWorkPeriod();
	void InitDialog(class CPrevYearsHolidaysDlg *pDlg);
	void OnGuiChange(int iYear, CPrevYearsHolidaysDlg *pDlg);
	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);

private:
	void SetYearsByWorkPeriod();

	CMyTime mFirstInPeriod;
	CMyTime mLastInPeriod;
	CString msHolidaysSelection;

	bool mbDefinedBySpecialDialog;

	void UpdateSum(CPrevYearsHolidaysDlg *pDlg);

	CList <class CHolidaysDuePerYear*, CHolidaysDuePerYear*> mHolidaysPerYer;
	CHolidaysDuePerYear mSum;
};

extern CHolidaysDue gHolidaysDue;

