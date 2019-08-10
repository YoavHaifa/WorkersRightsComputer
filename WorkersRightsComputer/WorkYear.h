#pragma once
#include "MyTime.h"

class CWorkYear
{
public:
	CWorkYear(void);
	~CWorkYear(void);

	int mSeniority;
	CMyTime mFirstDay;
	CMyTime mLastDay;
	CMyTime mDayAfter;
	bool mbLast;
	int mnUnpaidVacationDays;
	int mnAllCalendarDays;
	int mnPaidCalendarDays;
	double mFraction; // Only the last year may be < 1

	void InitFirst(void);
	void InitNext(CWorkYear &prev);
	void InitInternals(void);
	int GetNFullMonths(int* pnExtraDays, double* pExtraDaysFraction);
	bool Contains(class CHoliday& holiday);
	bool Contains(CMyTime &time);

	void AddUnpaidVacation(class CVacationUsed& vacation);
	void Log(FILE* pfLog);
	FILE* mpfLog;
	int mDebug;
};

