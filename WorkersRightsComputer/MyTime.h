#pragma once

class CMyTime
{
public:
	CMyTime();
	CMyTime(CTime time);
	CMyTime(int year, int month, int day);
	~CMyTime();

	bool IsValid();
	void SetNow();
	void Set(CTime time);
	void SetDate(CTime time);
	void Set(int year, int month, int day);
	void Set(__int64 t64);

	void Read(FILE *pf);
	void Write(FILE* pf);
	void WriteInLine(FILE* pf);
	void Log(FILE* pf);
	void LogLine(FILE* pf, const wchar_t*zText);
	void Reset();
	bool operator > (const CMyTime &other)const;
	bool operator < (const CMyTime &other)const;
	bool operator <= (const CMyTime &other)const;
	bool operator >= (const CMyTime& other)const;
	bool operator == (const CMyTime& other)const;
	CMyTime & operator = (const CTime &time);

	CTime mTime;
	int mYear;
	int mMonth;
	int mDay;
	int mHour; // Just for debug - should be always 0!
	int mDayOfWeek;
	void AddYears(int n);
	void AddMonth();
	void AddMonths(int n);
	void AddMonths(CMyTime &oldTime, int n);
	void AddDays(int n);
	void SubDay();
	void AddDay();
	CMyTime NextDay();
	CMyTime PrevDay();
	CMyTime NextMonth();
	CMyTime FirstDayOfMonth()const;
	CMyTime LastDayOfMonth()const;
	CTimeSpan Subtract(CMyTime &other);
	CString ToString();
	CString ToMonthString();
	CString ToHebrewString();
	bool mbInitialized;
	CString GetShortString();
	int GetNMonthsUntil(CMyTime& lastDay, int* pnExtraDays = NULL);
	int GetNMonthsBefore(CMyTime& dayAfter, int* pnExtraDays = NULL);
	double GetNWeeksUntil(CMyTime& lastDay);
	int GetNDaysUntil(CMyTime& lastDay);
	int GetNDaysBefore(CMyTime& dayAfter);
	bool IsWorkingDay();
	double AdvanceToNextMonth();
	void ToPrevMonth();
	bool IsOnPrevMonthsTo(CMyTime &time);
	bool MonthContains(CMyTime date);
	void SetClearDate();
	bool IsMonthBefore(int year, int month);
	bool IsMonthBefore(CMyTime &other);
	bool IsMonthSame(CMyTime &other);
	CMyTime GetMonthAfter() const;
	CMyTime GetMonthBefore() const;
};

