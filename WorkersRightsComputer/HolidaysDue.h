#pragma once
#include "afxtempl.h"
#include "HolidaysDuePerYear.h"

class CHolidaysDue
{
public:
	CHolidaysDue();

	void InitDialog(class CPrevYearsHolidaysDlg *pDlg);
	void OnGuiChange(int iYear, CPrevYearsHolidaysDlg *pDlg);
	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);

private:
	void UpdateSum(CPrevYearsHolidaysDlg *pDlg);

	CList <class CHolidaysDuePerYear*, CHolidaysDuePerYear*> mHolidaysPerYer;
	CHolidaysDuePerYear mSum;
};

extern CHolidaysDue gHolidaysDue;

