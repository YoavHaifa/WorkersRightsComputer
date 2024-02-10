#include "StdAfx.h"
#include "WorkYear.h"
#include "WorkPeriod.h"
#include "Utils.h"
#include "Config.h"
#include "Holidays.h"

CWorkYear::CWorkYear(void)
	: mSeniority(0)
	, mnUnpaidVacationCalendarDaysForSeverance(0)
	, mpPrev(NULL)
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
	mpPrev = NULL;
	mSeniority = 1;
	InitInternals(gWorkPeriod.mFirst);
}
void CWorkYear::InitNext(CWorkYear& prev)
{
	mpPrev = &prev;
	mSeniority = prev.mSeniority + 1;
	InitInternals(prev.mDayAfter);
}
void CWorkYear::InitInternals(CMyTime& firstDay)
{
	if (mpfLog)
		mFirstDay.LogLine(mpfLog, L"<InitInternals> mFirstDay ");

	CMyTime dayAfter(firstDay.mYear+1, firstDay.mMonth, firstDay.mDay);
	InitWorkSpanWithVacations(firstDay, dayAfter);
	if (mLastDay > gWorkPeriod.mLast)
	{
		CUtils::MessBox(L"<CWorkYear::InitInternals> last day of year beyond work period - corrected", L"SW Warning");
		mLastDay = gWorkPeriod.mLast;
	}
	if (mLastDay == gWorkPeriod.mLast)
	{
		mLastDay.LogLine(mpfLog, L"<InitInternals> reached last day in work period");
		mbLast = true;
	}

	mFraction = 1;
	mnUnpaidVacationCalendarDaysForSeverance = 0;
	if (mpfLog)
	{
		mLastDay.LogLine(mpfLog, L"last day %s");
		fwprintf(mpfLog, L"mnAllCalendarDays %d\n", mnDays);
		fwprintf(mpfLog, L"mnPaidCalendarDays %d\n", mnPaidCalendarDays);
	}

	// Compute fraction
	if (mnPaidCalendarDays >= gConfig.umnDaysInNormalYear)
	{
		mFraction = 1;
		if (mnPaidCalendarDays > gConfig.umnMaxDaysInYear)
			CUtils::MessBox(L"<CWorkYear::InitInternals> mnPaidCalendarDays > 366", L"SW Error");
	}
	else
	{
		mFraction = (double)mnPaidCalendarDays / gConfig.umnDaysInNormalYear;
		if (!mbLast)
		{
			// All the years other than the last should be full years
			// If there were unpaid vacations - the years end is postponed in intialization
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
	if (pExtraDaysFraction)
		*pExtraDaysFraction = (double)*pnExtraDays / (double)nDaysInFullMonth;
	return nMonths;
}
bool CWorkYear::Contains(CHoliday& holiday)
{
	if (holiday.mbAllYears)
	{
		CMyTime ht(mFirstDay.mYear, holiday.mMonth, holiday.mDay);
		while (ht < mDayAfter)
		{
			if (Contains(ht))
			{
				holiday.mYear = ht.mYear;
				return true;
			}
			ht.AddYears(1);
		}
		return false;
	}

	CMyTime ht(holiday.mYear, holiday.mMonth, holiday.mDay);
	return Contains(ht);
}
bool CWorkYear::Contains(CMyTime& time)
{
	if (time < mFirstDay)
		return false;
	if (time > mLastDay)
		return false;
	return true;
}
int CWorkYear::GetUnpaidVacationCalendarDaysForSeverance(void)
{
	mnUnpaidVacationCalendarDaysForSeverance = 0;
	int unpaidCalendarDays = mnDays - mnPaidCalendarDays;
	if (unpaidCalendarDays > 0)
	{
		int nToAdd = min(unpaidCalendarDays, MAX_14_UNPAID_VACATION_DAYS_FOR_SEVERANCE);

		double fractionThreshold = MIN_MONTHS_FOR_UNPAID_VACATION_DAYS_FOR_SEVERANCE / 12.0;
		if (mFraction > fractionThreshold)
			mnUnpaidVacationCalendarDaysForSeverance = nToAdd;
		else if (mpPrev)
		{
			if (mpPrev->mnUnpaidVacationCalendarDaysForSeverance)
			{
				int leftToAdd = MAX_14_UNPAID_VACATION_DAYS_FOR_SEVERANCE - mpPrev->mnUnpaidVacationCalendarDaysForSeverance;
				int mayAdd = (int)(leftToAdd + mFraction * MAX_14_UNPAID_VACATION_DAYS_FOR_SEVERANCE);
				mnUnpaidVacationCalendarDaysForSeverance = min(nToAdd, mayAdd);
			}
			else
				mnUnpaidVacationCalendarDaysForSeverance = nToAdd;
		}
	}
	return mnUnpaidVacationCalendarDaysForSeverance;
}
bool CWorkYear::HasFullYearUntil(CMyTime& lastDay)
{
	CMyTime first(mFirstDay);
	first.AddDays(mnUnpaidVacationDays);
	int nMonths = first.GetNMonthsUntil(lastDay);
	return nMonths >= 12;
}
double CWorkYear::GetVacationFraction()
{
	double daysForVacation = mnPaidCalendarDays - mnPaidMaternityDays;
	double vacationFraction = 0;
	if (mbLast)
		vacationFraction = min(1.0, daysForVacation / gConfig.umnDaysInNormalYear);
	else
		vacationFraction = daysForVacation / mnPaidCalendarDays;
	return vacationFraction;
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
		fprintf(pfLog, ", Unpaid Vacation %d/%d Days", mnUnpaidVacationDays, mnDays);
	else
		fprintf(pfLog, ", %d Days", mnDays);

	if (mnPaidMaternityDays > 0)
		fprintf(pfLog, ", Paid Maternity %d Days", mnPaidMaternityDays);

	if (mnUnpaidVacationCalendarDaysForSeverance > 0)
		fprintf(pfLog, ", Severance extra %d Days", mnUnpaidVacationCalendarDaysForSeverance);
	fprintf(pfLog, "\n");

	if (!mUnpaidSpans.IsEmpty())
	{
		fprintf(pfLog, "Unpaid: ");
		POSITION pos = mUnpaidSpans.GetHeadPosition();
		while (pos)
		{
			CDaysSpan* pSpan = mUnpaidSpans.GetNext(pos);
			pSpan->Log(pfLog);
		}
	}

	if (!mPaidMeternity.IsEmpty())
	{
		fprintf(pfLog, "Paid Maternity: ");
		POSITION pos = mPaidMeternity.GetHeadPosition();
		while (pos)
		{
			CDaysSpan* pSpan = mPaidMeternity.GetNext(pos);
			pSpan->Log(pfLog);
		}
	}
}
