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

	class CMonthlyRates *mpPensionRates;
	class CMonthlyRates* mpSeveranceMonthlyRates;

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
	bool mbEntitledOnlyToSeveranceFund;
	bool mbHadActivePensionBefore;
	CPensionReport mReport;

	bool UpdateStartDateForPensionForVacations(void);
	void WriteToLetter(class CHtmlWriter& html);

private:
	bool DoCompute(void);
	bool CheckForDateToStartPension();
	void PensionAddMonth(CMyTime &yearAndMonth, int nDays /* if 0 - full */, bool bFirst = false);
	void OnYearEnd(void);
};

extern CPension *gpPension;