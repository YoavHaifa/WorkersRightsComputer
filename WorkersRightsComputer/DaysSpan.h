#pragma once
#include "MyTime.h"

class CDaysSpan
{
public:
	CDaysSpan();
	CDaysSpan(CMyTime& firstDay, CMyTime& lastDay);
	CDaysSpan(CDaysSpan& other);

	void InitSpan(CMyTime& firstDay, CMyTime& lastDay);
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

