#include "stdafx.h"
#include "WorkSpan.h"
#include "WorkPeriod.h"
#include "UsedVacations.h"
#include "VacationUsed.h"
#include "MaternityLeave.h"
#include "Utils.h"


CWorkSpan::CWorkSpan(void)
	: mnUnpaidVacationDays(0)
	, mnPaidCalendarDays(0)
	, mbLast(false)
	, mpfLog(NULL)
{
}
void CWorkSpan::Init(CMyTime& firstDay, CMyTime& dayAfter)
{
	CDaysSpan::InitSpan(firstDay, dayAfter.PrevDay());

	mnUnpaidVacationDays = 0;
	mbLast = false;

	// Add unpaid vacations
	ClipByWorkPeriod();
	gUsedVacations.AddAllVacationsToWorkSpan(*this);

	mnPaidCalendarDays = mnDays - mnUnpaidVacationDays;
}
bool CWorkSpan::ClipByWorkPeriod()
{
	if (CDaysSpan::ClipByWorkPeriodEnd())
	{
		if (mpfLog)
			mLastDay.LogLine(mpfLog, L"CLIP LAST YEAR: last day %s");
		mbLast = true;
		return true;
	}
	return false;
}
void CWorkSpan::ComputeDays(void)
{
}
bool CWorkSpan::AddUnpaidVacation(CDaysSpan& unpaidSpan)
{
	if (mpfLog)
	{
		fwprintf(mpfLog, L"<AddUnpaidVacation>\n");
		unpaidSpan.Log(mpfLog);
	}

	mUnpaidSpans.AddTail(new CDaysSpan(unpaidSpan));
	mLastDay.AddDays(unpaidSpan.mnDays);
	InitSpan(mFirstDay, mLastDay);
	mnUnpaidVacationDays += unpaidSpan.mnDays;
	if (mpfLog)
		fwprintf(mpfLog, L"<AddUnpaidVacation> added %d unpaid --> %d\n",
			unpaidSpan.mnDays, mnUnpaidVacationDays);
	return true;
}
void CWorkSpan::AddMaternityLeave(CMaternityLeave& maternityLeave)
{

}