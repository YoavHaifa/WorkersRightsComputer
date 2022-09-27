#pragma once
#include "MyTime.h"

typedef enum EWageMode
{
	WAGE_UNDEF,
	WAGE_MIN,
	WAGE_MONTHLY,
	WAGE_HOURLY,
	WAGE_LAST
} EWageMode;

class CWagePeriod
{
public:
	CWagePeriod(EWageMode eMode);

	bool Is(EWageMode eMode)const { return meMode == eMode; }
	EWageMode Mode()const { return meMode; }

	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);

	void SetFirst();
	void SetLast();

	CString GetStateText();

private:
	friend class CWage;

	CMyTime mFirst;
	CMyTime mLast;

	EWageMode meMode; // Must be "MIN", "MONTHLY" or "HOURLY"

	double mMonthlyWage;
	double mHourlyWage;
	double mHoursPerMonth;
};

