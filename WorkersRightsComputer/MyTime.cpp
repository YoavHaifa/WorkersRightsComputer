#include "stdafx.h"
#include "MyTime.h"
#include "WorkPeriod.h"


CMyTime::CMyTime()
	: mYear(0)
	, mMonth(0)
	, mDay(0)
	, mDayOfWeek(0)
	, mTime(0)
	, mbInitialized(false)
{
}
CMyTime::CMyTime(int year, int month, int day)
	: mYear(year)
	, mMonth(month)
	, mDay(day)
	, mDayOfWeek(0)
	, mTime(0)
	, mbInitialized(false)
{
	CTime time(year, month, day, 0, 0, 0);
	Set(time);
}
CMyTime::~CMyTime()
{
}
void CMyTime::Set(CTime time)
{
	mTime = time;
	mYear = time.GetYear();
	mMonth = time.GetMonth();
	mDay = time.GetDay();
	mHour = time.GetHour();
	mDayOfWeek = time.GetDayOfWeek();
	mbInitialized = true;
}
void CMyTime::SetDate(CTime time)
{
	Set(time);
	SetClearDate();
}
void CMyTime::Set(int year, int month, int day)
{
	CTime time(year, month, day, 0, 0, 0);
	Set(time);
}
void CMyTime::Set(__int64 t64)
{
	CTime newTime(t64);
	Set(newTime);
}
void CMyTime::Read(FILE *pf)
{
	if (!pf)
		return;
	__int64 time64;
	//int nSet = fscanf_s(pf, "%I64d", &time64, sizeof(time64));
	int nSet = fwscanf_s(pf, L"%I64d", &time64);
	if (nSet == 1)
	{
		if (time64 > 0)
			Set(time64);
		else
			mbInitialized = false;
	}
}
void CMyTime::Write(FILE *pf)
{
	if (mbInitialized)
		fwprintf(pf, L"%I64d\n", (__int64)mTime.GetTime());
	else
		fwprintf(pf, L"0\n");
}
void CMyTime::Reset(void)
{
	mbInitialized = false;
	mYear = 0;
	mMonth = 0;
	mDay = 0;
}
void CMyTime::AddYears(int n)
{
	Set(mYear + n, mMonth, mDay);
}
void CMyTime::AddMonth(void)
{
	if (mMonth >= 12)
	{
		mYear++;
		mMonth = 1;
	}
	else
		mMonth++;
	Set(mYear, mMonth, mDay);
}
void CMyTime::AddMonths(int n)
{
	if (n >= 12)
	{
		mYear += n / 12;
		n = n % 12;
	}
	mMonth += n;
	while (mMonth > 12)
	{
		mYear++;
		mMonth -= 12;
	}
	Set(mYear, mMonth, mDay);
}
void CMyTime::AddMonths(CMyTime &oldTime, int n)
{
	Set(oldTime.mTime);
	AddMonths(n);
}
void CMyTime::AddDays(int n)
{
	while (n-- > 0)
		AddDay();
}
void CMyTime::AddDay(void)
{
	int lastDay = mDay;
	int lastHour = mTime.GetHour();
	CTimeSpan span(1, 0, 0, 0);
	Set(mTime + span);

	// Correct for changes due to daylight saving...
	if (mDay == lastDay)
	{
		CTimeSpan span1Day(0, 1, 0, 0);
		Set(mTime + span1Day);
	}
	else
	{
		int newHour = mTime.GetHour();
		if (newHour > lastHour)
		{
			CTimeSpan spanDiff(0, lastHour - newHour, 0, 0);
			Set(mTime + spanDiff);
		}
	}
}
CMyTime CMyTime::NextDay(void)
{
	CMyTime next = *this;
	next.AddDay();
	return next;
}
void CMyTime::SubDay(void)
{
	CTimeSpan spanMinusDay(-1, 0, 0, 0);
	Set(mTime + spanMinusDay);

	// Correct for changes due to daylight saving...
	if (mHour == 1)
	{
		CTimeSpan span(0, -1, 0, 0);
		Set(mTime + span);
	}
	
	if (mHour == 23)
	{
		CTimeSpan span(0, 1, 0, 0);
		Set(mTime + span);
	}
}
CMyTime & CMyTime::operator = (CTime &time)
{
	Set(time);
	return *this;
}
bool CMyTime::operator < (CMyTime &other)
{
	return other > *this;
}
bool CMyTime::operator > (CMyTime &other)
{
	if (mYear > other.mYear)
		return true;
	if (mYear < other.mYear)
		return false;
	if (mMonth > other.mMonth)
		return true;
	if (mMonth < other.mMonth)
		return false;
	if (mDay > other.mDay)
		return true;
	return false;
}
bool CMyTime::operator >= (CMyTime &other)
{
	return other <= *this;
}
bool CMyTime::operator <= (CMyTime &other)
{
	if (mYear > other.mYear)
		return false;
	if (mYear < other.mYear)
		return true;
	if (mMonth > other.mMonth)
		return false;
	if (mMonth < other.mMonth)
		return true;
	if (mDay > other.mDay)
		return false;
	return true;
}
CTimeSpan CMyTime::Subtract(CMyTime &other)
{
	return mTime - other.mTime;
}
CString CMyTime::ToString(void)
{
	CString s = mTime.Format(_T("%A, %B %d, %Y"));
	return s;
}
CString CMyTime::GetShortString()
{
	CString s = mTime.Format(_T("%B %d, %Y"));
	return s;
}
int CMyTime::GetNDaysUntil(CMyTime &lastDay)
{
	CTimeSpan span = lastDay.mTime - mTime;
	return (int)span.GetDays() + 1;
}
bool CMyTime::IsWorkingDay()
{
	if (mDayOfWeek < 1)
		return false;
	return (gWorkPeriod.maWorkingDays[mDayOfWeek - 1] > 0);
}
double CMyTime::AdvanceToNextMonth(void)
{
	if (mDay == 1)
	{
		AddMonth();
		return 1;
	}

	int nDaysInMonthStart = mDay - 1;
	int month = mMonth;
	int countDays = 0;

	while (mMonth == month)
	{
		countDays++;
		AddDay();
	}
	return ((double)countDays / (countDays + nDaysInMonthStart));
}
bool CMyTime::IsOnPrevMonthsTo(CMyTime &other)
{
	if (mYear < other.mYear)
		return true;
	if (mYear > other.mYear)
		return false;
	return mMonth < other.mMonth;
}
bool CMyTime::MonthContains(CMyTime date)
{
	return (mYear == date.mYear && mMonth == date.mMonth);
}
void CMyTime::SetClearDate()
{
	Set(mYear, mMonth, mDay);
}
