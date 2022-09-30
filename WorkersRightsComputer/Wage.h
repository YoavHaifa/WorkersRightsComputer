#pragma once
#include "WagePeriod.h"

class CWage
{
public:
	CWage();

	void SetMinWage(void);
	void SetMonthlyWage(double wage);
	void SetHourlyWage(double wagePerHour, double nHoursPerWeek);

	//void Restore(FILE* pfRead);
	bool IsSinglePeriod() { return mPeriods.GetSize() == 1; }
	bool IsSinglePeriod(EWageMode& oeMode);

	void AddPeriod(class CWagePeriod*);

	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pMain);

	double GetMonthlyWage();
	double GetHourlyWage();
	double GetHoursPerMonth();

	CString GetStateDesc();

	int mDebug;

private:
	class CWagePeriod* mpFirst;
	class CWagePeriod* mpLast;
	void SetSingle(EWageMode eMode);
	CList<class CWagePeriod*, class CWagePeriod*> mPeriods;
	void Clear();
	void ClearOverlap(CWagePeriod& newPeriod);
	void Update();
};

extern CWage gWage;

