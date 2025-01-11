#pragma once
#include "afxtempl.h"
#include "HolidaysDuePerYear.h"
#include "MyTime.h"

class CHolidaysDue
{
public:
	CHolidaysDue();

	void Reset();
	bool VerifyWorkPeriod();
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

	bool RequiresDefintionByDay() { return mbHolidaysByDay; }

	bool mbNoHolidays;
	bool mbHolidaysByDay;
	bool mbHolidaysRelative;

private:
	void SetInvisible();
	void DecideModeByMainDlg();

	void LoadFromOldXml(CXMLParseNode* pRoot);
	void SetYearsByWorkPeriod();

	CMyTime mFirstInPeriod;
	CMyTime mLastInPeriod;
	CString msHolidaysSelection;

	bool mbPeriodAndHolidaysDefined;
	bool mbDefinedBySpecialDialog;

	void UpdateSum(CMyDialogEx* pDlg = NULL);

	int mn;
	CList <class CHolidaysDuePerYear*, CHolidaysDuePerYear*> mHolidaysPerYears;
	CHolidaysDuePerYear mSum;
	CHolidaysDuePerYear mSumPrev;
	CHolidaysDuePerYear* mpThisYear;

	static const int N_MAIN_DLG_FIELDS = 11;
	unsigned int maMainDlgFields[N_MAIN_DLG_FIELDS];
};

extern CHolidaysDue gHolidaysDue;

