#pragma once

#include "right.h"


class CVacation : public CRight
{
public:
	CVacation(void);

	virtual	bool SetCheckRef(CButtonRef *pButton) override;
	virtual	bool SetEditRef(class CEditRef *pRef) override;

	static const int MIN_DAYS_FOR_SHORT_RULE = 75;
	static const int SHORT_RULE_PER_CENT = 4;
	static const int MAX_FULL_YEARS = 3;
	static const int N_MONTH_FOR_FULL_YEARLY_VACATION = 10;
	static const double MIN_FRACTION_FOR_ROUND_UP;

	CEdit *mpPrevYearsBox;

	double mnYearsForVacation;
	double mnMonthsForVacation;
	double mPayPerDay;
	int mnSeniority;

	bool mbUseShortRule;
	double mnDueDays;
	CButtonRef * mpbDemandForPreviousYears;
	double mnYearsOfUnpaidVacation;
	double mnDaysOfPaidVacation;
	CString msDesc;

	//int GetNDaysPerYear(int seniority, double nDaysPerWeek);
	virtual bool Compute(void) override;
	bool ComputePerPeriod(void);
	virtual CString GetDecriptionForLetter(void)override;
	void PrepareString(void);

	int mCurYear;
	int mCurMonth;
	void CountBackMonth(void);
};

