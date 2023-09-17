#include "stdafx.h"
#include "DaysSpan.h"
#include "WorkPeriod.h"

CDaysSpan::CDaysSpan()
	: mnDays(0)
{
}
CDaysSpan::CDaysSpan(CMyTime& firstDay, CMyTime& lastDay)
{
	InitSpan(firstDay, lastDay);
}
CDaysSpan::CDaysSpan(CDaysSpan& other)
{
	InitSpan(other.mFirstDay, other.mLastDay);
}
void CDaysSpan::InitSpan(CMyTime& firstDay, CMyTime& lastDay)
{
	mFirstDay = firstDay;
	mLastDay = lastDay;
	mDayAfter = mLastDay.NextDay();
	mnDays = mFirstDay.GetNDaysBefore(mDayAfter);
}
bool CDaysSpan::ClipByWorkPeriodEnd()
{
	if (mDayAfter > gWorkPeriod.mLast)
	{
		InitSpan(mFirstDay, gWorkPeriod.mLast);
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
bool CDaysSpan::Contains(CDaysSpan& other)
{
	if (mnDays < 1) // Empty
		return false;

	if (mFirstDay > other.mFirstDay) // Other starts before
		return false;
	if (mLastDay < other.mLastDay) // Other extends after
		return false;
	return true; // Contains
}
bool CDaysSpan::Contains(CMyTime& day)
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
	oCommon.InitSpan(first, last);
	return oCommon.mnDays > 0;
}
void CDaysSpan::Log(FILE* pf)
{
	fprintf(pf, "%d/%d/%d - %d/%d/%d\n",
		mFirstDay.mDay, mFirstDay.mMonth, mFirstDay.mYear, 
		mLastDay.mDay, mLastDay.mMonth, mLastDay.mYear);
}