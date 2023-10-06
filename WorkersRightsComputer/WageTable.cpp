#include "stdafx.h"
#include "WageTable.h"
#include "WorkPeriod.h"
#include "Wage.h"
#include "Utils.h"

CWageTable gWageTable;

CWageTable::CWageTable()
	: mn(0)
	, miFirst(-1)
	, miLast(-2)
	, mDebug(15)
{
}
void CWageTable::Clear()
{
	mn = 0;
	miFirst = -1;
	miLast = -2;

	memset(&maWage[0][0], 0, sizeof(maWage));
	memset(&mapPeriod[0][0], 0, sizeof(mapPeriod));
}
bool CWageTable::IsValid(void)
{
	static int unErr = 0;
	bool bValid = (mn > 0) && (miFirst >= 0) && (miLast == (miFirst + mn - 1));
	if (!bValid && unErr < 2)
	{
		Log(L"Unvalid");
		CUtils::MessBox(L"Invalid wage table", L"SW Error");
		unErr++;
	}
	return bValid;
}
double CWageTable::ComputeHolidayPrice(const CMyTime& date)
{
	double baseWage = ComputeHolidayPriceBaseWage(date);

	double extraHour = 0;
	gWorkPeriod.mbExtraHolidayHoursForLiveInApplied = false;
	if (gWorkPeriod.mbLiveIn)
	{
		if (date.mYear > 2016 || (date.mYear == 2016 && date.mMonth > 6))
		{
			extraHour = baseWage / gWorkPeriod.GetWorkingHoursInFullMonth(date);
			gWorkPeriod.mbExtraHolidayHoursForLiveInApplied = true;
		}
	}
	return (baseWage / gWorkPeriod.mnDaysInMonthForDailySalary + extraHour) * 1.5;
}
double CWageTable::ComputeHolidayPriceBaseWage(const CMyTime& date)
{
	int iYear = date.mYear - YEAR0;
	return maWage[iYear][date.mMonth - 1];
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

	return ComputeMonthlyPay(gWorkPeriod.mLast);
}
double CWageTable::PayPerDayAtWorkEnd(void)
{
	if (mn < 1)
		return 0;

	double payPerMonth = PayPerMonthAtWorkEnd();
	return (payPerMonth / gWorkPeriod.mnDaysInMonthForDailySalary);
}
double CWageTable::ComputeMonthlyPay(const CMyTime &date)
{
	if (mn < 1)
		return 0;
	int iYear = date.mYear - YEAR0;
	return maWage[iYear][date.mMonth - 1];
}
bool CWageTable::Prepare(const wchar_t* zAt)
{
	Clear();

	mn = 0;
	POSITION pos = gWage.GetHeadPosition();
	while (pos)
	{
		CWagePeriod* pPeriod = gWage.GetNext(pos);
		CMyTime time = pPeriod->GetFirstMonth();
		do
		{
			int iYear = time.mYear - YEAR0;
			int iMonth = time.mMonth - 1;
			maWage[iYear][iMonth] = pPeriod->GetMonthlyWage(time);
			mapPeriod[iYear][iMonth] = pPeriod;
			mn++;

			miLast = iYear * N_MONTHS + iMonth;
			if (mn == 1)
				miFirst = miLast;

		} while (pPeriod->GetNextMonth(time));
	}
	if (mDebug)
		Log(zAt);
	return IsValid();
}
void CWageTable::Log(const wchar_t* zAt)
{
	FILE* pfLog = CUtils::TryOpenLogFile(L"WageTable", zAt);
	if (!pfLog)
		return;

	int n = 0;
	double prevWage = 0;
	CWagePeriod* pPrevPeriod = NULL;

	bool bIn = false;
	int nUnprinted = 0;
	for (int iYear = 0; iYear < N_YEARS; iYear++)
	{
		for (int iMonth = 0; iMonth < N_MONTHS; iMonth++)
		{
			CWagePeriod* pPeriod = mapPeriod[iYear][iMonth];
			if (pPeriod)
			{
				bIn = true;
				n++;
				double wage = maWage[iYear][iMonth];
				if (wage != prevWage || pPeriod != pPrevPeriod)
				{
					CString sMode(pPeriod->GetSMode());
					fwprintf(pfLog, L"%3d: y %4d m %2d - %8.2f - %s\n", n,
						iYear + YEAR0, iMonth + 1, wage, (const wchar_t*)sMode);
					prevWage = wage;
					pPrevPeriod = pPeriod;
					nUnprinted = 0;
				}
				else
					nUnprinted++;
			}
			else if (bIn)
			{
				iYear = N_YEARS;
				break;
			}
		}
	}

	if (nUnprinted > 0)
	{
		CMyTime last = pPrevPeriod->GetLastMonth();
		CString sMode(pPrevPeriod->GetSMode());
		fwprintf(pfLog, L"%3d: y %4d m %2d - %8.2f - %s (last month)\n", n,
			last.mYear, last.mMonth, prevWage, (const wchar_t*)sMode);
	}

	fprintf(pfLog, "\nWage table defined for %d months\n", n);
	fclose(pfLog);
}
