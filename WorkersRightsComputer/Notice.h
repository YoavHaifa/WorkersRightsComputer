#pragma once

#include "right.h"
#include "MyTime.h"

class CNotice : public CRight
{
public:
	CNotice(void);
	virtual ~CNotice(void);
	virtual bool Compute(void) override;
	virtual CString GetDecriptionForLetter(void)override;
	virtual CString GetDecriptionForLetterHebrew(void)override;

	bool mbDemandFullMonthAnyway;
	bool mbDemandFullMonth;
	double mDueNoticeDays;

	double mDueWorkDayToPay;
	double mPayPerDay;

	int mnDaysPaidAfterNotice;
	CMyTime mLastDayOfNotice;
};

