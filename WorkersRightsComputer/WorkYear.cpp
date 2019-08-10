#include "StdAfx.h"
#include "WorkYear.h"
#include "WorkPeriod.h"
#include "UsedVacations.h"
#include "VacationUsed.h"
#include "Utils.h"
#include "Holidays.h"

CWorkYear::CWorkYear(void)
	: mSeniority(0)
	, mbLast(false)
	, mnUnpaidVacationDays(0)
	, mpfLog(NULL)
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
	mFirstDay = gWorkPeriod.mFirst;
	InitInternals();
}
void CWorkYear::InitNext(CWorkYear& prev)
{
	mSeniority = prev.mSeniority + 1;
	mFirstDay = prev.mDayAfter;
	InitInternals();
}
void CWorkYear::InitInternals(void)
{
	if (mpfLog)
		mFirstDay.LogLine(mpfLog, L"<InitInternals> mFirstDay ");

	mnUnpaidVacationDays = 0;
	mbLast = false;
	mDayAfter = mFirstDay;
	mDayAfter.AddYears(1);
	mLastDay = mDayAfter.PrevDay();
	mFraction = 1;
	if (mpfLog)
	{
		mLastDay.LogLine(mpfLog, L"last day %s");
		mnAllCalendarDays = mFirstDay.GetNDaysBefore(mDayAfter);
		fwprintf(mpfLog, L"mnAllCalendarDays %d\n", mnAllCalendarDays);
	}

	// Add unpaid vacations
	gUsedVacations.AddToWorkYear(*this);

	if (mDayAfter > gWorkPeriod.mLast)
	{
		if (mpfLog)
			mLastDay.LogLine(mpfLog, L"CLIP LAST YEAR: last day %s");

		mbLast = true;
		mLastDay = gWorkPeriod.mLast;
		mDayAfter = mLastDay.NextDay();
	}

	// Compute number of days
	mnAllCalendarDays = mFirstDay.GetNDaysBefore(mDayAfter);
	mnPaidCalendarDays = mnAllCalendarDays - mnUnpaidVacationDays;
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
int CWorkYear::GetNFullMonths(int *pnExtraDays)
{
	CMyTime first(mFirstDay);
	first.AddDays(mnUnpaidVacationDays);
	return first.GetNMonthsBefore(mDayAfter, pnExtraDays);
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
void CWorkYear::AddUnpaidVacation(class CVacationUsed& vacation)
{
	static int count = 0;
	count++;

	int nAllCalendarDays = 0;
	if (mpfLog)
	{
		mDayAfter.LogLine(mpfLog, L"\n<AddUnpaidVacation> day after ");
		fwprintf(mpfLog, L"<AddUnpaidVacation count %d> %d days\n", count, vacation.mnUnpaidCalendarDays);
		nAllCalendarDays = mFirstDay.GetNDaysBefore(mDayAfter);
		fwprintf(mpfLog, L"nAllCalendarDays %d\n", mnAllCalendarDays);
		if (count == 3)
			fwprintf(mpfLog, L"Beware:\n");
	}
	mDayAfter.AddDays(vacation.mnUnpaidCalendarDays);
	mLastDay = mDayAfter.PrevDay();
	mnUnpaidVacationDays += vacation.mnUnpaidCalendarDays;
	if (mpfLog)
	{
		fwprintf(mpfLog, L"<AddUnpaidVacation> %d days\n", vacation.mnUnpaidCalendarDays);
		mDayAfter.LogLine(mpfLog, L"day after ");
		mLastDay.LogLine(mpfLog, L"last day ");
		int nNewAllCalendarDays = mFirstDay.GetNDaysBefore(mDayAfter);
		fwprintf(mpfLog, L"nNewAllCalendarDays %d\n", nNewAllCalendarDays);
		fwprintf(mpfLog, L"\n");
		if (nNewAllCalendarDays != nAllCalendarDays + vacation.mnUnpaidCalendarDays)
		{
			fflush(mpfLog);
			CUtils::MessBox(L"nNewAllCalendarDays != nAllCalendarDays + vacation.mnUnpaidCalendarDays", L"SW Error");
		}
		fwprintf(mpfLog, L"\n");
	}
}
void CWorkYear::Log(FILE* pfLog)
{
	if (!pfLog)
		return;
	fprintf(pfLog, "%2d: ", mSeniority);
	mFirstDay.Log(pfLog);
	fprintf(pfLog, "- ");
	mLastDay.Log(pfLog);
	if (mnUnpaidVacationDays > 0)
		fprintf(pfLog, "Unpaid Vacation %d Days", mnUnpaidVacationDays);
	fprintf(pfLog, "\n");
}
