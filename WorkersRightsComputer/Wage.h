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
	// bool IsSinglePeriod() { return mPeriods.GetSize() == 1; }
	bool IsSinglePeriod(EWageMode& oeMode);

	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pMain);

	double GetMonthlyWage();
	double GetHourlyWage();
	double GetHoursPerMonth();

	CString GetStateText();

private:
	void SetSingle(EWageMode eMode);
	CList<class CWagePeriod*, class CWagePeriod*> mPeriods;
	void Clear();
	void Update();
};

extern CWage gWage;

