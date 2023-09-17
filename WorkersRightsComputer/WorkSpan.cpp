#include "stdafx.h"
#include "WorkSpan.h"
#include "WorkPeriod.h"
#include "UsedVacations.h"
#include "VacationUsed.h"
#include "MaternityLeave.h"
#include "Utils.h"


CWorkSpan::CWorkSpan()
	: mnUnpaidVacationDays(0)
	, mnPaidMaternityDays(0)
	, mnPaidCalendarDays(0)
	, mbLast(false)
	, mpfLog(NULL)
{
}
CWorkSpan::~CWorkSpan()
{
	Clear();
}
void CWorkSpan::Clear()
{
	while (!mUnpaidSpans.IsEmpty())
	{
		delete mUnpaidSpans.GetTail();
		mUnpaidSpans.RemoveTail();
	}

	while (!mPaidMeternity.IsEmpty())
	{
		delete mPaidMeternity.GetTail();
		mPaidMeternity.RemoveTail();
	}
}
void CWorkSpan::Init(CMyTime& firstDay, CMyTime& dayAfter)
{
	Clear();
	CDaysSpan::InitSpan(firstDay, dayAfter.PrevDay());

	mnUnpaidVacationDays = 0;
	mnPaidMaternityDays = 0;
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
	if (mLastDay > gWorkPeriod.mLast)
		mLastDay = gWorkPeriod.mLast;

	InitSpan(mFirstDay, mLastDay);
	mnUnpaidVacationDays += unpaidSpan.mnDays;
	if (mpfLog)
		fwprintf(mpfLog, L"<AddUnpaidVacation> added %d unpaid --> %d\n",
			unpaidSpan.mnDays, mnUnpaidVacationDays);
	return true;
}
void CWorkSpan::AddPaidMaternityLeave(CDaysSpan& paidSpan)
{
	CDaysSpan common;
	if (Intersect(paidSpan, common))
	{
		mPaidMeternity.AddTail(new CDaysSpan(common));
		mnPaidMaternityDays += common.mnDays;
	}
}