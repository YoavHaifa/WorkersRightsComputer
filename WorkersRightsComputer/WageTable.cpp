#include "stdafx.h"
#include "WageTable.h"
#include "WorkPeriod.h"
#include "Wage.h"

CWageTable gWageTable;

CWageTable::CWageTable()
	: mn(0)
	, miFirst(-1)
	, miLast(-2)
{
}
bool CWageTable::IsValid(void)
{
	return (mn > 0) && (miFirst >= 0) && (miLast == (miFirst + mn - 1));
}
double CWageTable::ComputeHolidayPrice(int year, int month)
{
	double baseWage = ComputeHolidayPriceBaseWage(year, month);

	double extraHour = 0;
	gWorkPeriod.mbExtraHolidayHoursForLiveInApplied = false;
	if (gWorkPeriod.mbLiveIn)
	{
		if (year > 2016 || (year == 2016 && month > 6))
		{
			extraHour = baseWage / gWorkPeriod.GetWorkingHoursInFullMonth(year, month);
			gWorkPeriod.mbExtraHolidayHoursForLiveInApplied = true;
		}
	}
	return (baseWage / gWorkPeriod.mnDaysInMonthForDailySalary + extraHour) * 1.5;
}
double CWageTable::ComputeHolidayPriceBaseWage(int year, int month)
{
	int iYear = year - YEAR0;
	return maWage[iYear][month - 1];
	/*
	for (int i = 0; i < mn; i++)
	{
		if (
			(map[i]->mYear < year)
			||
			((map[i]->mYear == year) && (map[i]->mMonth <= month))
			)
		{
			return map[i]->mWage;
		}
	}

	return map[mn - 1]->mWage;*/
}
double CWageTable::PayPerMonthAtWorkEnd(void)
{
	if (mn < 1)
		return 0;

	return ComputeMonthlyPay(gWorkPeriod.mLast.mYear, gWorkPeriod.mLast.mMonth);
}
double CWageTable::PayPerDayAtWorkEnd(void)
{
	if (mn < 1)
		return 0;

	double payPerMonth = PayPerMonthAtWorkEnd();
	return (payPerMonth / gWorkPeriod.mnDaysInMonthForDailySalary);
}
double CWageTable::ComputeMonthlyPay(int year, int month)
{
	if (mn < 1)
		return 0;
	int iYear = year - YEAR0;
	return maWage[iYear][month - 1];

	/*
	for (int i = 0; i < mn; i++)
	{
		if (
			(map[i]->mYear < year)
			||
			((map[i]->mYear == year) && (map[i]->mMonth <= month))
			)
		{
			return map[i]->mWage;
		}
	}

	return (map[mn - 1]->mWage);*/
}
