#include "stdafx.h"
#include "DaysSpan.h"
#include "WorkPeriod.h"

CDaysSpan::CDaysSpan()
	: mnDays(0)
	, mnWorkDays(0)
{
}
CDaysSpan::CDaysSpan(const CMyTime& firstDay, const CMyTime& lastDay)
{
	InitDaysSpan(firstDay, lastDay);
}
CDaysSpan::CDaysSpan(const CDaysSpan& other)
{
	InitDaysSpan(other.mFirstDay, other.mLastDay);
}
void CDaysSpan::InitDaysSpan(const CMyTime& firstDay, const CMyTime& lastDay)
{
	mFirstDay = firstDay;
	mLastDay = lastDay;
	mDayAfter = mLastDay.NextDay();
	mnDays = mFirstDay.GetNDaysBefore(mDayAfter);
	mnWorkDays = gWorkPeriod.CountWorkingDays(mFirstDay, mLastDay);
}
void CDaysSpan::SetMonth(const CMyTime& date)
{
	mFirstDay = date;
	mFirstDay.mDay = 1;

	mDayAfter = mFirstDay.NextMonth();
	mLastDay = mDayAfter.PrevDay();
	InitDaysSpan(mFirstDay, mLastDay);
}
bool CDaysSpan::ClipByWorkPeriodEnd()
{
	if (mDayAfter > gWorkPeriod.mLast)
	{
		InitDaysSpan(mFirstDay, gWorkPeriod.mLast);
		return true;
	}
	return false;
}
bool CDaysSpan::Overlaps(CDaysSpan& other)
{
	if (mnDays < 1 || other.mnDays < 1) // Empty
		return false;

	if (mLastDay < other.mFirstDay) // Before
		return false;
	if (mFirstDay > other.mLastDay) // After
		return false;
	return true; // Overlap
}
bool CDaysSpan::Contains(const CDaysSpan& other)
{
	if (mnDays < 1) // Empty
		return false;

	if (mFirstDay > other.mFirstDay) // Other starts before
		return false;
	if (mLastDay < other.mLastDay) // Other extends after
		return false;
	return true; // Contains
}
bool CDaysSpan::Contains(const CMyTime& day)
{
	if (mnDays < 1) // Empty
		return false;
	if (mFirstDay > day) // day is before
		return false;
	if (mLastDay < day) // day is after
		return false;
	return true; // Contains
}
bool CDaysSpan::Intersect(CDaysSpan& other, CDaysSpan& oCommon)
{
	CMyTime first = mFirstDay;
	if (mFirstDay < other.mFirstDay)
		first = other.mFirstDay;
	CMyTime last = mLastDay;
	if (mLastDay > other.mLastDay)
		last = other.mLastDay;
	oCommon.InitDaysSpan(first, last);
	return oCommon.mnDays > 0;
}
void CDaysSpan::UpdateMonthlyInfo4Unpaid(bool bNoPension)
{
	int nUnpaidToAssign = mnWorkDays;

	// Update for relevant months in work period
	CMyTime updateMonthStart(mFirstDay.mYear, mFirstDay.mMonth, 1);
	while (nUnpaidToAssign > 0)
	{
		CMonthInfo* pMonthInfo = gWorkPeriod.GetMonthInfoFor(updateMonthStart);
		CDaysSpan common;
		if (Intersect(*pMonthInfo, common))
		{
			int nRelevantWorkDaysPerMonth = pMonthInfo->GetNRelevantDaysIn(common, bNoPension);
			if (nRelevantWorkDaysPerMonth > 0)
			{
				int nToAssign = min(nUnpaidToAssign, nRelevantWorkDaysPerMonth);
				if (bNoPension)
					pMonthInfo->SetNoPension(nToAssign);
				else
					pMonthInfo->SetUnpaid(nToAssign);

				nUnpaidToAssign -= nToAssign;
			}
		}
		if (pMonthInfo->mbLast)
			break;
		updateMonthStart.AddMonth();
	}
}
void CDaysSpan::Log(FILE* pf, const char *zText)
{
	if (zText)
		fprintf(pf, "%s: ", zText);

	CString sFirst(mFirstDay.ToString());
	CString sLast(mLastDay.ToString());
	fwprintf(pf, L"%s - %s (work days %d/%d)\n",
		(const wchar_t*)sFirst, (const wchar_t*)sLast, mnWorkDays, mnDays);
}