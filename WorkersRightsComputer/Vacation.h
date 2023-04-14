#pragma once

#include "right.h"


class CVacation : public CRight
{
public:
	CVacation(void);

	virtual	bool SetCheckRef(CButtonRef *pButton) override;
	virtual	bool SetEditRef(class CEditRef *pRef) override;
	virtual bool MissingInput(CString& iosText) override;

	static const int MIN_DAYS_FOR_SHORT_RULE = 75;
	static const int SHORT_RULE_PER_CENT = 4;
	static const int MAX_FULL_YEARS = 3;

	//static const int N_MONTH_FOR_FULL_YEARLY_VACATION = 12; // Changed according to Iris, 6 August 2021 -
	// No need for special rule to treat partial last year as full year (v1.1.1)
	// Use config value instead - so that it can be set later without SW change
	//static const int N_MONTH_FOR_FULL_YEARLY_VACATION = 10;

	static const double MIN_FRACTION_FOR_ROUND_UP;

	CEdit* mpPrevYearsBox;
	CEdit* mpLastYearsPaidDaysBox;

	double mnYearsForVacation;
	double mnMonthsForVacation;
	double mPayPerDay;
	int mnSeniority;

	bool mbUseShortRule;
	bool mbLastYearAllPaid;
	bool mbLastYearDaysPaid;
	bool mnLastYearDaysPaid;
	double mnDueDays;
	CButtonRef* mpbDemandForPreviousYears;
	CButtonRef* mpbAllPaid4LastYear;
	CButtonRef* mpbDaysPaid4LastYear;
	double mnYearsOfUnpaidVacation;
	double mnDaysOfPaidVacation;
	double mnDaysPaidLastYear;
	CString msDesc;
	CString msDescHebrew;

	//int GetNDaysPerYear(int seniority, double nDaysPerWeek);
	virtual bool Compute(void) override;
	bool ComputePerLastYear();
	bool ComputePerPeriod(void);
	virtual CString GetDecriptionForLetter(void)override;
	virtual CString GetDecriptionForLetterHebrew(void)override;
	void PrepareString(void);

	int mCurYear;
	int mCurMonth;
	void CountBackMonth(void);

	bool GetGUIValue(CEdit* pEdit, double& oValue, CButtonRef* pButton);
	void ComputePrevYears(void);
	void ComputeLastYearByShortRule(void);
};

