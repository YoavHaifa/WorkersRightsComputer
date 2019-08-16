#include "StdAfx.h"
#include "WorkYear.h"
#include "WorkPeriod.h"
#include "Utils.h"
#include "Holidays.h"

CWorkYear::CWorkYear(void)
	: mSeniority(0)
	, mDebug(0)
{
	if (mDebug)
	{
		static int count = 0;
		wchar_t zBuf[128];
		swprintf_s(zBuf, 128, L"c:\\MyLog\\WorkYear_%d.log", count++);
		mpfLog = MyFOpenWithErrorBox(zBuf, L"w", L"Log");
	}
}
CWorkYear::~CWorkYear(void)
{
	if (mpfLog)
		fclose(mpfLog);
}
void CWorkYear::InitFirst(void)
{
	mSeniority = 1;
	InitInternals(gWorkPeriod.mFirst);
}
void CWorkYear::InitNext(CWorkYear& prev)
{
	mSeniority = prev.mSeniority + 1;
	InitInternals(prev.mDayAfter);
}
void CWorkYear::InitInternals(CMyTime& firstDay)
{
	if (mpfLog)
		mFirstDay.LogLine(mpfLog, L"<InitInternals> mFirstDay ");

	CMyTime dayAfter(firstDay.mYear+1, firstDay.mMonth, firstDay.mDay);
	Init(firstDay, dayAfter);

	mFraction = 1;
	if (mpfLog)
	{
		mLastDay.LogLine(mpfLog, L"last day %s");
		fwprintf(mpfLog, L"mnAllCalendarDays %d\n", mnAllCalendarDays);
	}

	if (mpfLog)
	{
		fwprintf(mpfLog, L"mnAllCalendarDays %d\n", mnAllCalendarDays);
		fwprintf(mpfLog, L"mnPaidCalendarDays %d\n", mnPaidCalendarDays);
	}

	// Comput fraction
	if (mnPaidCalendarDays >= 365)
	{
		mFraction = 1;
		if (mnPaidCalendarDays > 366)
			CUtils::MessBox(L"<CWorkYear::InitInternals> mnPaidCalendarDays > 366", L"SW Error");
	}
	else
	{
		mFraction = (double)mnPaidCalendarDays / 365.0;
		if (!mbLast)
		{
			static int countErrors = 0;
			if (mpfLog)
			{
				fwprintf(mpfLog, L"<CWorkYear::InitInternals> SW Error: a year wrongly identified as fraction\n\n");
				fflush(mpfLog);
			}
			CUtils::MessBox(L"<CWorkYear::InitInternals> a year wrongly identified as fraction", L"SW Error");
			countErrors++;
		}
	}
}
int CWorkYear::GetNFullMonths(int *pnExtraDays, double* pExtraDaysFraction)
{
	CMyTime first(mFirstDay);
	first.AddDays(mnUnpaidVacationDays);
	int nMonths = first.GetNMonthsBefore(mDayAfter, pnExtraDays);
	double nDaysInFullMonth = 30;
	if (mDayAfter.mMonth == 3)
	{
		if (*pnExtraDays >= mDayAfter.mDay)
			nDaysInFullMonth = 28;
	}
	*pExtraDaysFraction = (double)* pnExtraDays / nDaysInFullMonth;
	return nMonths;
}
bool CWorkYear::Contains(CHoliday& holiday)
{
	if (holiday.mbAllYears)
	{
		CMyTime h(mFirstDay.mYear, holiday.mMonth, holiday.mDay);
		while (h < mDayAfter)
		{
			if (Contains(h))
			{
				holiday.mYear = h.mYear;
				return true;
			}
			h.AddYears(1);
		}
		return false;
	}

	CMyTime h(holiday.mYear, holiday.mMonth, holiday.mDay);
	return Contains(h);
}
bool CWorkYear::Contains(CMyTime& time)
{
	if (time < mFirstDay)
		return false;
	if (time > mLastDay)
		return false;
	return true;
}
void CWorkYear::Log(FILE* pfLog)
{
	if (!pfLog)
		return;
	fprintf(pfLog, "%2d: ", mSeniority);
	mFirstDay.Log(pfLog);
	fprintf(pfLog, "- ");
	mLastDay.Log(pfLog);

	if (mFraction < 1)
		fprintf(pfLog, ", Fraction %.3f", mFraction);

	if (mnUnpaidVacationDays > 0)
		fprintf(pfLog, ", Unpaid Vacation %d/%d Days", mnUnpaidVacationDays, mnAllCalendarDays);
	else
		fprintf(pfLog, ", %d Days", mnAllCalendarDays);

	fprintf(pfLog, "\n");
}
