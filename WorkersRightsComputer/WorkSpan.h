#pragma once
#include "DaysSpan.h"

class CWorkSpan : public CDaysSpan
{
public:
	CWorkSpan(void);
	//CWorkSpan(CMyTime& firstDay, CMyTime& dayAfter);

	void Init(CMyTime& firstDay, CMyTime& dayAfter);
	bool AddUnpaidVacation(CDaysSpan& unpaidSpan);
	void AddMaternityLeave(class CMaternityLeave& maternityLeave);
	bool ClipByWorkPeriod();
	void ComputeDays();

	int mnUnpaidVacationDays;
	int mnPaidCalendarDays;
	bool mbLast;

	CList<CDaysSpan*, CDaysSpan*> mUnpaidSpans;

	// Debug
	FILE* mpfLog;
};

