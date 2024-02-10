#pragma once

struct CWageLevel
{
	int mYear;
	int mMonth;
	double mWage;

	void Log(FILE* pf)
	{
		fprintf(pf, "%4d %2d - %.2f\n", mYear, mMonth, mWage);
	}
};

class CWageTable
{
public:
	CWageTable();

	void Clear();
	bool IsValid();
	bool Prepare(const wchar_t* zAt);
	void Log(const wchar_t* zAt);

	double PayPerMonthAtWorkEnd();
	double PayPerDayAtWorkEnd();
	double ComputeMonthlyPay(const class CMyTime &date);
	double ComputeHolidayPrice(const class CMyTime& date);

private:
	double ComputeHolidayPriceBaseWage(const class CMyTime& date);

	int mn;
	static const int YEAR0 = 1950;
	static const int N_MONTHS = 12;
	static const int N_YEARS = 200;
	double maWage[N_YEARS][N_MONTHS];
	class CWagePeriod *mapPeriod[N_YEARS][N_MONTHS];
	int miFirst;
	int miLast;

	int mDebug;
	//CWageLevel** map;
};

extern CWageTable gWageTable;