#pragma once
#include "WorkPeriod.h"

struct CLevel
{
	int mYear;
	int mMonth;
	double mWage;

	void Log(FILE *pf)
	{
		fprintf(pf, "%4d %2d - %.2f\n", mYear, mMonth, mWage);
	}
};

class CMinWage
{
public:
	CMinWage(void);

	bool IsValid(void);

	bool InitFromFile(void);
	void PrintLog(void);
	double ComputeHolidayPriceBaseWage(int year, int month);
	double ComputeHolidayPrice(int year, int month);
	double PayPerMonthAtWorkEnd(void);
	double PayPerDayAtWorkEnd(void);
	double ComputeMonthlyPay(int year, int month);

	int mn;
	CLevel **map;

};

extern CMinWage gMinWage;
