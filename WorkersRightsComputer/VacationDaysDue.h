#pragma once

class CVacationDaysDue
{
public:
	CVacationDaysDue();

	bool VerifyWorkPeriod(class CMyDialogEx* pMainDlg);
	void Reset();
	void InitDialog(class CMyDialogEx* pDlg);

	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);

private:
	void SetYearsByWorkPeriod();

	int mn;
	bool mbAskForFullYears;
	int mnFullYears;
	bool mbAskForPartialYears;
	int mnDaysDue;

	CList <class CVacationDuePerYear*, CVacationDuePerYear*> mVacationPerYears;
	CVacationDuePerYear* mpThisYearVacationDays;
};

extern CVacationDaysDue gVacationDaysDue;
