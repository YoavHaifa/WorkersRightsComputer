#include "StdAfx.h"
#include "Severance.h"
#include "WorkPeriod.h"
#include "WorkYears.h"
#include "MinWage.h"
#include "FamilyPart.h"
#include "UsedVacations.h"


CSeverance::CSeverance(void)
	: CRight(L"Severance", L"פיצויי פיטורים")
	, mpbAllowSeveranceForShorterPeriod(NULL)
	, mpbEntitledOnlyToSeveranceFund(NULL)
{
	miPrintOrder = 1;
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
		mpbEntitledOnlyToSeveranceFund = pButton;
		return true;
	}
	return false;
}
bool CSeverance::Compute(void)
{
	if (mpbEntitledOnlyToSeveranceFund->IsChecked())
	{
		mDuePay = 0;
		msDue += L"Entitle ONLY to Severance Fund - None here";
		return false;
	}

	mnYears = gWorkYears.mYearsForSeverance;
	mPayPerYear = gMinWage.ComputeMonthlyPay(gWorkPeriod.mLast.mYear, gWorkPeriod.mLast.mMonth);
	if (mnYears < MIN_PERIOD)
	{
		//double partYearThreshold = (double)N_MIN_MONTHS_FOR_SPECIAL_CASE / 12 - 0.002;
		//if (mpbAllowSeveranceForShorterPeriod->IsChecked() && mnYears >= partYearThreshold)
		if (mpbAllowSeveranceForShorterPeriod->IsChecked() && gWorkYears.mbAllowPartYearSeverance)
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
	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		mDuePay = mDuePay * gFamilyPart.mRatio;
		msDue += L" =FamilyPart=> ";
		msDue += ToString(mDuePay);

	}

	if (gUsedVacations.mbAdd14DaysUnpaidVacation4Severance)
	{
		msDebug += L" Including ";
		msDebug += ToString(gWorkYears.mnDaysForSeveranceAddedForUnpaidVacations);
		msDebug += L" days of unpaid vacation";
	}
	else
		msDebug += L" 14 days of unpaid vacation were not requested";
	return true;
}
CString CSeverance::GetDecriptionForLetter(void)
{
	CString s = ToString(mnYears, 3);
	s += L" years * ";
	s += ToString(mPayPerYear);

	return s;
}
CString CSeverance::GetDecriptionForLetterHebrew(void)
{
	CString s = ToString(mnYears, 3);
	s += L" שנים * ";
	s += ToString(mPayPerYear);

	return s;
}
