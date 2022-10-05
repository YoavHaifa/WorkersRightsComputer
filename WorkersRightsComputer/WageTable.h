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

	double PayPerMonthAtWorkEnd(void);
	double PayPerDayAtWorkEnd(void);
	double ComputeMonthlyPay(int year, int month);
	double ComputeHolidayPrice(int year, int month);

private:
	double ComputeHolidayPriceBaseWage(int year, int month);

	int mn;
	CWageLevel** map;
};

extern CWageTable gWageTable;