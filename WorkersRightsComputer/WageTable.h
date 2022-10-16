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
	bool IsValid(void);
	void Prepare();

	double PayPerMonthAtWorkEnd(void);
	double PayPerDayAtWorkEnd(void);
	double ComputeMonthlyPay(int year, int month);
	double ComputeHolidayPrice(int year, int month);

private:
	double ComputeHolidayPriceBaseWage(int year, int month);

	int mn;
	static const int YEAR0 = 1950;
	static const int N_MONTHS = 12;
	static const int N_YEARS = 200;
	double maWage[N_YEARS][N_MONTHS];
	class CWagePeriod *mapPeriod[N_YEARS][N_MONTHS];
	int miFirst;
	int miLast;
	//CWageLevel** map;
};

extern CWageTable gWageTable;