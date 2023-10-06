#pragma once
#include "MyTime.h"

class CDaysSpan
{
public:
	CDaysSpan();
	CDaysSpan(CMyTime& firstDay, CMyTime& lastDay);
	CDaysSpan(CDaysSpan& other);

	void InitDaysSpan(CMyTime& firstDay, CMyTime& lastDay);
	//void InitDaysSpanWithVacations(CMyTime& firstDay, CMyTime& lastDay);
	void SetMonth(const CMyTime &date);

	bool ClipByWorkPeriodEnd();

	bool Overlaps(CDaysSpan& other);
	bool Contains(CDaysSpan& other);
	bool Contains(CMyTime& day);
	bool Intersect(CDaysSpan& other, CDaysSpan& oCommon);

	void UpdateMonthlyInfo4Unpaid(bool bNoPension);

	void Log(FILE* pf, const char* zText=NULL);

	CMyTime mFirstDay;
	CMyTime mLastDay;
	CMyTime mDayAfter;
	int mnDays;
	int mnWorkDays;
};

