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
	void AddMonth(int year, int month, int nDays /* if 0 - full */, bool bFirst = false);
	void OnYearEnd(void);

	class CMonthlyRates *mpPensionRates;
	class CYearlyRates *mpSeveranceRates;

	static const int N_MONTHS_BEFORE_PAY_FROM_2008 = 9;
	static const int N_MONTHS_BEFORE_PAY_FROM_2009 = 6;
	static const int N_MONTHS_BEFORE_PAY_CONTINUITY = 3;
	static const int YEAR_TO_START = 2008;
	static const int YEAR_TO_START_CHECKING_VACATIONS = 2010;

	CMyTime mStartDateForPension;
	CButtonRef * mpbEntitledOnlyToSeveranceFund;
	CButtonRef * mpbHadActivePensionBefore;
	bool mbCollectiveAgreementWithDifferentRates;

	double mPensionPerYear;
	double mSeverancePerYear;
	double mPensionDue;
	double mSeveranceDue;
	double mDueFromFamily;
	bool mbSeverance;
	CPensionReport mReport;

	bool UpdateStartDateForPension(void);
	void WriteToLetter(class CHtmlWriter& html);
	void CorrectForOldStype(void);
};

extern CPension *gpPension;