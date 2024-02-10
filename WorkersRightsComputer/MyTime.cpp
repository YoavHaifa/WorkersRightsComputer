#include "stdafx.h"
#include "MyTime.h"
#include "WorkPeriod.h"
#include "Utils.h"

unsigned int SafeGetDaysTime(CTime start, CTime end)
{
	// Discard hours, minutes, seconds, and daylight savings time
	CTime startDay(start.GetYear(), start.GetMonth(), start.GetDay(), 0, 0, 0, 0);
	CTime endDay(end.GetYear(), end.GetMonth(), end.GetDay(), 0, 0, 0, 0);

	// Get number of days apart
	CTimeSpan span = endDay - startDay;
	return (unsigned int)span.GetDays();
}
unsigned int SafeGetDays(CMyTime &start, CMyTime &end)
{
	return SafeGetDaysTime(start.mTime, end.mTime);
}

CMyTime::CMyTime()
	: mYear(0)
	, mMonth(0)
	, mDay(0)
	, mDayOfWeek(0)
	, mTime(0)
	, mbInitialized(false)
{
}
CMyTime::CMyTime(CTime time)
	: mYear(0)
	, mMonth(0)
	, mDay(0)
	, mDayOfWeek(0)
	, mTime(0)
	, mbInitialized(false)
{
	Set(time);
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
bool CMyTime::IsValid()
{
	if (!mbInitialized)
		return false;
	if (mYear == 0)
		return false;
	return true;
}
void CMyTime::SetNow()
{
	Set(CTime::GetCurrentTime());
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
void CMyTime::WriteInLine(FILE* pf)
{
	if (mbInitialized)
		fwprintf(pf, L"%I64d", (__int64)mTime.GetTime());
	else
		fwprintf(pf, L"0");
}
void CMyTime::Log(FILE* pf)
{
	CString s(ToString());
	fwprintf(pf, L"%s ", (const wchar_t *)s);
}
void CMyTime::LogLine(FILE* pf, const wchar_t* zText)
{
	if (!pf)
		return;

	CString s(ToString());
	if (mHour != 0)
		fwprintf(pf, L"%s %s (hour %d)\n", zText, (const wchar_t *)s, mHour);
	else
		fwprintf(pf, L"%s %s\n", zText, (const wchar_t *)s);
}
void CMyTime::Reset()
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
CMyTime CMyTime::NextMonth()
{
	CMyTime nextMonth(*this);
	nextMonth.AddMonth();
	return nextMonth;
}
CMyTime CMyTime::FirstDayOfMonth()const
{
	CMyTime firstDay(mYear, mMonth, 1);
	return firstDay;
}
CMyTime CMyTime::LastDayOfMonth()const
{
	CMyTime lastDay(*this);
	lastDay.mDay = 1;
	lastDay.AddMonth();
	lastDay.SubDay();
	return lastDay;
}
void CMyTime::AddMonth()
{
	int wantedDay = mDay;
	int wantedMonth = mMonth + 1;
	if (wantedMonth > 12)
	{
		mYear++;
		wantedMonth = 1;
	}
	Set(mYear, wantedMonth, wantedDay);
	while (mMonth != wantedMonth)
		Set(mYear, wantedMonth, --wantedDay);
}
void CMyTime::AddMonths(int nMonthsToAdd)
{
	if (nMonthsToAdd >= 12)
	{
		mYear += nMonthsToAdd / 12;
		nMonthsToAdd = nMonthsToAdd % 12;
	}
	mMonth += nMonthsToAdd;
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
void CMyTime::AddDay()
{
	int lastDay = mDay;
	int lastHour = mTime.GetHour();
	CTimeSpan span1Day(1, 0, 0, 0);
	Set(mTime + span1Day);

	// Correct for changes due to daylight saving...
	if (mDay == lastDay)
	{
		CTimeSpan span1hour(0, 1, 0, 0);
		Set(mTime + span1hour);
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
CMyTime CMyTime::NextDay()
{
	CMyTime next = *this;
	next.AddDay();
	return next;
}
CMyTime CMyTime::PrevDay()
{
	CMyTime prev = *this;
	prev.SubDay();
	return prev;
}
void CMyTime::SubDay()
{
	CMyTime saveDate(*this);
	CTimeSpan spanMinusDay(1, 0, 0, 0);
	__int64 saveTime = mTime.GetTime();
	Set(mTime - spanMinusDay);
	__int64 newTime = mTime.GetTime();
	__int64 diff = saveTime - newTime;

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
bool CMyTime::operator <= (CMyTime& other)
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
bool CMyTime::operator == (CMyTime& other)
{
	if (mYear != other.mYear)
		return false;
	if (mMonth != other.mMonth)
		return false;
	if (mDay != other.mDay)
		return false;
	return true;
}
CTimeSpan CMyTime::Subtract(CMyTime &other)
{
	return mTime - other.mTime;
}
CString CMyTime::ToString()
{
	CString s = mTime.Format(_T("%A, %B %d, %Y"));
	return s;
}
CString CMyTime::ToMonthString()
{
	CString s = mTime.Format(_T("%m.%Y"));
	return s;
}
CString CMyTime::ToHebrewString()
{
	CString s = mTime.Format(_T("%d.%m.%Y"));
	return s;
}
CString CMyTime::GetShortString()
{
	CString s = mTime.Format(_T("%B %d, %Y"));
	return s;
}
int CMyTime::GetNMonthsUntil(CMyTime& lastDay, int* pnExtraDays)
{
	CMyTime dayAfter = lastDay.NextDay();
	return GetNMonthsBefore(dayAfter, pnExtraDays);
}
int CMyTime::GetNMonthsBefore(CMyTime& dayAfter, int* pnExtraDays)
{
	CMyTime countDays(*this);
	int nMonths = 0;
	int nDays = 0;
	int prevday = countDays.mDay;

	while (countDays < dayAfter)
	{
		countDays.AddDay();
		if (countDays.mDay == mDay)
		{
			nMonths++;
			nDays = 0;
		}
		else if (countDays.mDay < prevday && prevday < mDay) // Shorter month ended
		{
			nMonths++;
			nDays = 0;
		}
		else
			nDays++;

		prevday = countDays.mDay;
	}
	if (pnExtraDays)
		*pnExtraDays = nDays;
	return nMonths;
}
int CMyTime::GetNDaysUntil(CMyTime &lastDay)
{
	return SafeGetDays(*this, lastDay) + 1;
}
double CMyTime::GetNWeeksUntil(CMyTime& lastDay)
{
	int nDays = GetNDaysUntil(lastDay);
	double nWeeks = nDays / 7.0;
	return nWeeks;
}
int CMyTime::GetNDaysBefore(CMyTime& dayAfter)
{
	return SafeGetDays(*this, dayAfter);
}
bool CMyTime::IsWorkingDay()
{
	if (mDayOfWeek < 1)
		return false;
	return (gWorkPeriod.maWorkingDays[mDayOfWeek - 1] > 0);
}
double CMyTime::AdvanceToNextMonth()
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
void CMyTime::ToPrevMonth()
{
	int month = mMonth;
	while (mMonth == month)
	{
		SubDay();
	}
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
bool CMyTime::IsMonthBefore(int year, int month)
{
	if (mYear < year)
		return true;
	if (mYear > year)
		return false;
	return mMonth < month;
}
bool CMyTime::IsMonthBefore(CMyTime &other)
{
	if (mYear < other.mYear)
		return true;
	if (mYear > other.mYear)
		return false;
	return mMonth < other.mMonth;
}
bool CMyTime::IsMonthSame(CMyTime &other)
{
	return (mYear == other.mYear) && (mMonth == other.mMonth);
}
CMyTime CMyTime::GetMonthAfter() const
{
	CMyTime nextMonth(*this);
	while (nextMonth.mMonth == mMonth)
	{
		nextMonth.AddDay();
	}
	return nextMonth;
}
CMyTime CMyTime::GetMonthBefore() const
{
	CMyTime prevMonth(*this);
	while (prevMonth.mMonth == mMonth)
	{
		prevMonth.SubDay();
	}
	return prevMonth;
}
