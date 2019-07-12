#pragma once

class CMyTime
{
public:
	CMyTime();
	CMyTime(CTime time);
	CMyTime(int year, int month, int day);
	~CMyTime();

	void SetNow(void);
	void Set(CTime time);
	void SetDate(CTime time);
	void Set(int year, int month, int day);
	void Set(__int64 t64);
	void Read(FILE *pf);
	void Write(FILE *pf);
	void Reset(void);
	bool operator > (CMyTime &other);
	bool operator < (CMyTime &other);
	bool operator <= (CMyTime &other);
	bool operator >= (CMyTime &other);
	CMyTime & operator = (CTime &time);

	CTime mTime;
	int mYear;
	int mMonth;
	int mDay;
	int mHour; // Just for debug - should be always 0!
	int mDayOfWeek;
	void AddYears(int n);
	void AddMonth(void);
	void AddMonths(int n);
	void AddMonths(CMyTime &oldTime, int n);
	void AddDays(int n);
	void SubDay(void);
	void AddDay(void);
	CMyTime NextDay(void);
	CTimeSpan Subtract(CMyTime &other);
	CString ToString(void);
	bool mbInitialized;
	CString GetShortString();
	int GetNDaysUntil(CMyTime &lastDay);
	bool IsWorkingDay();
	double AdvanceToNextMonth(void);
	bool IsOnPrevMonthsTo(CMyTime &time);
	bool MonthContains(CMyTime date);
	void SetClearDate();
	bool IsMonthBefore(int year, int month);
	bool IsMonthBefore(CMyTime &other);
	bool IsMonthSame(CMyTime &other);
};

