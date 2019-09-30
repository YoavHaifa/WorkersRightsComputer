#pragma once
#include "right.h"

class CSeverance : public CRight
{
public:
	CSeverance(void);

	virtual	bool SetCheckRef(CButtonRef *pButton) override;
	virtual bool Compute(void) override;

	virtual CString GetDecriptionForLetter(void)override;
	virtual CString GetDecriptionForLetterHebrew(void)override;

	CButtonRef * mpbEntitledOnlyToSeveranceFund;
	CButtonRef * mpbAllowSeveranceForShorterPeriod;
	static const int MIN_PERIOD = 1;

	double mnYears;
	double mPayPerYear;
};

