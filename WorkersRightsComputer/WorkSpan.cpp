#include "stdafx.h"
#include "WorkSpan.h"
#include "WorkPeriod.h"
#include "UsedVacations.h"
#include "VacationUsed.h"
#include "Utils.h"

CWorkSpan::CWorkSpan(void)
	: mnUnpaidVacationDays(0)
	, mnAllCalendarDays(0)
	, mnPaidCalendarDays(0)
	, mbLast(false)
	, mpfLog(NULL)
{
}
CWorkSpan::CWorkSpan(CMyTime& firstDay, CMyTime& dayAfter)
	: mnUnpaidVacationDays(0)
	, mnAllCalendarDays(0)
	, mnPaidCalendarDays(0)
	, mbLast(false)
	, mpfLog(NULL)
{
	Init(firstDay, dayAfter);
}
void CWorkSpan::Init(CMyTime& firstDay, CMyTime& dayAfter)
{
	mFirstDay = firstDay;
	mDayAfter = dayAfter;

	mnUnpaidVacationDays = 0;
	mLastDay = mDayAfter.PrevDay();
	mnAllCalendarDays = mFirstDay.GetNDaysBefore(mDayAfter);
	mnPaidCalendarDays = mnAllCalendarDays;
	mbLast = false;

	// Add unpaid vacations
	gUsedVacations.AddToWorkSpan(*this);
	ClipByWorkPeriod();
	ComputeDays();
}
bool CWorkSpan::ClipByWorkPeriod(void)
{
	if (mDayAfter > gWorkPeriod.mLast)
	{
		if (mpfLog)
			mLastDay.LogLine(mpfLog, L"CLIP LAST YEAR: last day %s");

		mbLast = true;
		mLastDay = gWorkPeriod.mLast;
		mDayAfter = mLastDay.NextDay();
		return true;
	}
	return false;
}
void CWorkSpan::ComputeDays(void)
{
	// Compute number of days
	mnAllCalendarDays = mFirstDay.GetNDaysBefore(mDayAfter);
	mnPaidCalendarDays = mnAllCalendarDays - mnUnpaidVacationDays;
}
void CWorkSpan::AddUnpaidVacation(CVacationUsed& vacation)
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
void CWorkSpan::AddMaternityLeave(class CMaternityLeave& maternityLeave)
{

}