#pragma once

#include "right.h"
#include "MyTime.h"

class CNotice : public CRight
{
public:
	CNotice();
	virtual ~CNotice();
	virtual bool Compute() override;
	virtual CString GetDecriptionForLetter()override;
	virtual CString GetDecriptionForLetterHebrew()override;

	bool ComputeDemandFullMonth();
	bool ComputeForLessThanAYear();
	void CheckForPaidDaysAfterNotice();
	void CountWorkDaysToPay();

	bool mbDemandFullMonth;
	double mDueNoticeDays;

	double mDueWorkDaysToPay;
	double mPayPerDay;

	int mnDaysPaidAfterNotice;
	CMyTime mLastDayOfNotice;
};

