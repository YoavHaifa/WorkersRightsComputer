#pragma once

#include "right.h"


class CVacation : public CRight
{
public:
	CVacation();

	virtual	bool SetCheckRef(CButtonRef *pButton) override;
	virtual	bool SetEditRef(class CEditRef *pRef) override;
	virtual bool MissingInput(CString& iosText) override;

	static const int MIN_DAYS_FOR_SHORT_RULE = 75;
	static const int SHORT_RULE_PER_CENT = 4;
	static const int MAX_FULL_YEARS = 3;

	static const double MIN_FRACTION_FOR_ROUND_UP;

	CEdit* mpPrevYearsBox;
	CEdit* mpThisYearsPaidDaysBox;

	double mnYearsForVacation;
	double mnMonthsForVacation;
	double mPayPerDay;
	int mnSeniority;

	bool mbUseShortRule;
	bool mbThisYearAllPaid;
	bool mbThisYearDaysPaid;
	double mnDueDays;
	CButtonRef* mpbDemandForPreviousYears;
	CButtonRef* mpbAllPaid4ThisYear;
	CButtonRef* mpbDaysPaid4ThisYear;
	double mnPrevYearsOfUnpaidVacation;
	double mnDaysOfPaidVacation;
	double mnDaysPaidThisYear;
	CString msDesc;
	CString msDescHebrew;

	//int GetNDaysPerYear(int seniority, double nDaysPerWeek);
	virtual bool Compute() override;
	bool ComputeInternals();
	bool ReducePaidDays();
	void ComputeBackByMonth();
	void ComputeBackByYears();
	void ComputeEpilogue();
	virtual CString GetDecriptionForLetter()override;
	virtual CString GetDecriptionForLetterHebrew()override;
	void PrepareString();

	int mCurYear;
	int mCurMonth;
	void CountBackMonth();

	bool GetGUIValue(CEdit* pEdit, double& oValue, CButtonRef* pButton);
	void GetNPrevYears();
	bool ComputeByShortRule();
};

