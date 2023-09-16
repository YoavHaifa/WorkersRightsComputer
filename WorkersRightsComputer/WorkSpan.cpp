#include "stdafx.h"
#include "WorkSpan.h"
#include "WorkPeriod.h"
#include "UsedVacations.h"
#include "VacationUsed.h"
#include "MaternityLeave.h"
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
	ClipByWorkPeriod();
	gUsedVacations.AddAllVacationsToWorkSpan(*this);
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
bool CWorkSpan::AddUnpaidVacation(CVacationUsed& vacation)
{
	if (mpfLog)
	{
		fwprintf(mpfLog, L"<AddUnpaidVacation>\n");
		vacation.LongLog(mpfLog);
	}

	bool bClipped = false;
	CMyTime firstUnpaidDay = vacation.mFirstDayUnpaid;
	if (firstUnpaidDay < mFirstDay)
	{
		firstUnpaidDay = mFirstDay;
		bClipped = true;
	}
	CMyTime lastUnpaidDay = vacation.mLastDay;
	if (lastUnpaidDay > mLastDay)
	{
		lastUnpaidDay = mLastDay;
		bClipped = true;
	}
	int nUpaidDays = firstUnpaidDay.GetNDaysUntil(lastUnpaidDay);

	if (nUpaidDays > 0)
	{
		mVacations.AddTail(&vacation);
		mnUnpaidVacationDays += nUpaidDays;
		if (mpfLog)
			fwprintf(mpfLog, L"<AddUnpaidVacation> added %d unpaid --> %d (%s)\n",
				nUpaidDays, mnUnpaidVacationDays, bClipped ? L"Clipped" : L"Whole");
		return true;
	}

	if (mpfLog)
		fwprintf(mpfLog, L"<AddUnpaidVacation> nothing added!\n");
	return false;
}
void CWorkSpan::AddMaternityLeave(CMaternityLeave& maternityLeave)
{

}