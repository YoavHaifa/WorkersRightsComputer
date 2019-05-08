#include "StdAfx.h"
#include "Severance.h"
#include "WorkPeriod.h"
#include "MinWage.h"


CSeverance::CSeverance(void)
	: CRight(L"Severance", L"פיצויי פיטורים")
	, mpbAllowSeveranceForShorterPeriod(NULL)
	, mpbEntitledToSeveranceFund(NULL)
{
}
bool CSeverance::SetCheckRef(CButtonRef *pButton)
{
	if (pButton->msName == L"AllowSevLess")
	{
		mpbAllowSeveranceForShorterPeriod = pButton;
		return true;
	}
	if (pButton->msName == L"EntitledOnly2Sev")
	{
		mpbEntitledToSeveranceFund = pButton;
		return true;
	}
	return false;
}
bool CSeverance::Compute(void)
{
	if (mpbEntitledToSeveranceFund->IsChecked())
	{
		mDuePay = 0;
		msDue += L"Entitle ONLY to Severance Fund - None here";
		return false;
	}

	mnYears = gWorkPeriod.mYearsForSeverance;
	mPayPerYear = gMinWage.ComputeMonthlyPay(gWorkPeriod.mLast.mYear, gWorkPeriod.mLast.mMonth);
	if (mnYears < MIN_PERIOD)
	{
		double partYearThreshold = (double)N_MIN_MONTHS_FOR_SPECIAL_CASE / 12 - 0.002;
		if (mpbAllowSeveranceForShorterPeriod->IsChecked() && mnYears >= partYearThreshold)
		{
			mDuePay = mPayPerYear;
			msDue += L"Full pay for one year ==> ";
			msDue += ToString(mPayPerYear);
			return true;
		}
		else
		{
			mDuePay = 0;
			msDue += L"None - Period too short";
			return false;
		}
	}

	mDuePay = (int)(mnYears * mPayPerYear + 0.5);

	msDue += L"Pay for ";
	msDue += ToString(mnYears ,3);
	msDue += L" years * ";
	msDue += ToString(mPayPerYear);
	msDue += L" ==> ";
	msDue += ToString(mDuePay);
	return true;
}
CString CSeverance::GetDecriptionForLetter(void)
{
	CString s = ToString(mnYears, 3);
	s += L" years * ";
	s += ToString(mPayPerYear);
	
	return s;
}
