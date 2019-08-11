#pragma once
#include "MyTime.h"

class CWorkSpan
{
public:
	CWorkSpan(void);
	CWorkSpan(CMyTime& firstDay, CMyTime& dayAfter);

	void Init(CMyTime &firstDay, CMyTime &dayAfter);
	void AddUnpaidVacation(class CVacationUsed& vacation);
	bool ClipByWorkPeriod(void);
	void ComputeDays(void);

	CMyTime mFirstDay;
	CMyTime mLastDay;
	CMyTime mDayAfter;
	int mnUnpaidVacationDays;
	int mnAllCalendarDays;
	int mnPaidCalendarDays;
	bool mbLast;

	// Debug
	FILE* mpfLog;
};

