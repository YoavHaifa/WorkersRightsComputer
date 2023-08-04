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
	CWagePeriod(EWageMode eMode, const CMyTime& first, const CMyTime& last, double wage = 0, double nHours = 0);
	CWagePeriod(const CWagePeriod &other);
	CWagePeriod(class CXMLParseNode* pNode);

	bool IsValid();
	void Copy(const CWagePeriod &other);

	bool Is(EWageMode eMode)const { return meMode == eMode; }
	EWageMode Mode()const { return meMode; }

	void SaveToXml(class CXMLDump& xmlDump);
	void LoadFromXml(class CXMLParseNode* pRoot);

	void SetFirst();
	void SetLast();

	CString GetStateTextLine();
	bool MayUniteWith(const CWagePeriod& other);
	bool ComesJustAfter(const CWagePeriod& other);
	bool Check();

	CMyTime GetFirstMonth() { return mFirst; }
	CMyTime GetLastMonth() { return mLast; }
	bool GetNextMonth(CMyTime &ioTime);

	double GetMonthlyWage(const CMyTime &time);
	CString GetSMode();

private:
	friend class CWage;
	friend class CWagePeriodsDlg;

	static int umNewId;
	int mId;

	CMyTime mFirst;
	CMyTime mLast;

	EWageMode meMode; // Must be "MIN", "MONTHLY" or "HOURLY"

	double mMonthlyWage;
	double mHourlyWage;
	double mnHoursPerMonth;
	double mMonthlyBonus;
};

