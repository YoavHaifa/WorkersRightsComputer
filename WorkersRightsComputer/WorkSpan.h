#pragma once
#include "DaysSpan.h"

class CWorkSpan : public CDaysSpan
{
public:
	CWorkSpan();
	~CWorkSpan();

	void InitWorkSpan(CMyTime& firstDay, CMyTime& dayAfter);
	void InitWorkSpanWithVacations(CMyTime& firstDay, CMyTime& dayAfter);

	bool AddUnpaidVacation(CDaysSpan& unpaidSpan);
	void AddPaidMaternityLeave(CDaysSpan& paidSpan);
	bool ClipByWorkPeriod();
	void ComputeDays();
	void Clear();

	int mnUnpaidVacationDays;
	int mnPaidMaternityDays;
	int mnPaidCalendarDays;
	bool mbLast;

	CList<CDaysSpan*, CDaysSpan*> mUnpaidSpans;
	CList<CDaysSpan*, CDaysSpan*> mPaidMeternity;

protected:
	// Debug
	FILE* mpfLog;
};

