#pragma once

#include "right.h"
#include "PensionReport.h"


class CPension : public CRight
{
public:
	CPension(void);
	virtual ~CPension(void);
	virtual	bool SetCheckRef(CButtonRef *pButton) override;
	virtual bool Compute(void) override;
	virtual CString GetDecriptionForLetter(void)override;
	virtual CString GetDecriptionForLetterHebrew(void)override;

	bool DoCompute(void);
	void AddMonth(int year, int month, int nDays /* if 0 - full */);
	void OnYearEnd(void);

	class CMonthlyRates *mpPensionRates;
	class CYearlyRates *mpSeveranceRates;

	static const int N_MONTHS_BEFORE_PAY_FROM_2008 = 9;
	static const int N_MONTHS_BEFORE_PAY_FROM_2009 = 6;
	static const int N_MONTHS_BEFORE_PAY_CONTINUITY = 3;
	static const int YEAR_TO_START = 2008;

	CMyTime mStartDateForPension;
	CButtonRef * mpbEntitledToSeveranceFund;
	CButtonRef * mpbHadActivePensionBefore;
	bool mbCollectiveAgreementWithDifferentRates;

	double mPensionPerYear;
	double mSeverancePerYear;
	double mPensionDue;
	double mSeveranceDue;
	bool mbSeverance;
	CPensionReport mReport;

	void WriteToLetter(class CHtmlWriter& html);
};

extern CPension *gpPension;