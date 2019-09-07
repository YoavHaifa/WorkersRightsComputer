#pragma once
#include "MyTime.h"

class CVacationUsed
{
public:
	CVacationUsed(CMyTime firstDay, CMyTime lastDay);
	~CVacationUsed();

	CMyTime mFirstDay;
	CMyTime mFirstDayUnpaid;
	CMyTime mLastDay;
	CString GetText();
	int mnDays;
	int mnWorkDays;
	void Compute();
	int mnPaid;
	int mnUnPaid;
	int mnUnpaidCalendarDays;
	int mnUnpaidUsedForSeverance;
	void ShortLog(FILE *pf);
	void LongLog(FILE *pf);
	void SetPartiallyPaid(int nPaidDays);

	int CountDaysOfUnpaidVacation(CMyTime& first, CMyTime& last);
	void AddToWorkSpan(class CWorkSpan& workSpan);
};

