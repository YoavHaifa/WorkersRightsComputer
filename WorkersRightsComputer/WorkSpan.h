#pragma once
#include "DaysSpan.h"

class CWorkSpan : public CDaysSpan
{
public:
	CWorkSpan();
	~CWorkSpan();
	//CWorkSpan(CMyTime& firstDay, CMyTime& dayAfter);

	void Init(CMyTime& firstDay, CMyTime& dayAfter);
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

	// Debug
	FILE* mpfLog;
};

