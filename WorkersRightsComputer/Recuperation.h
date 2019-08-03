#pragma once

#include "right.h"
#include "Seniority.h"
#include "YearlyRates.h"


struct CRecVetek 
{
	int mYears;
	int mDaysDue;
};

class CRecuperation : public CRight
{
public:
	CRecuperation(void);
	virtual	bool SetCheckRef(CButtonRef *pButton) override;
	virtual	bool SetEditRef(class CEditRef *pRef) override;

	virtual bool Compute(void) override;
	virtual CString GetDecriptionForLetter(void)override;
	virtual CString GetDecriptionForLetterHebrew(void)override;

	CSeniority *mpSeniority;
	CYearlyRates *mpRates;

	static const int MONTH_OF_NEW_RATE = 7;
	static const int MAX_YEARS_BACK_WHILE_WORKING = 7;
	static const int MAX_YEARS_BACK_WHILE_NOT_WORKING = 2;
	static const int MIN_YEARS_TO_START = 1;

	CEdit *mpPrevYearsBox;
	CButtonRef *mpbDemandPreviousYears;
	double mnYearsBack;
	double mDueDays;
	double mRate;

};

