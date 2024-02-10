#include "StdAfx.h"
#include "Severance.h"
#include "WorkPeriod.h"
#include "WorkYears.h"
#include "WageTable.h"
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
		return true;
	}

	mnYears = gWorkYears.mYearsForSeverance;
	mPayPerYear = gWageTable.ComputeMonthlyPay(gWorkPeriod.mLast);
	if (mnYears < MIN_PERIOD)
	{
		//double partYearThreshold = (double)N_MIN_MONTHS_FOR_SPECIAL_CASE / 12 - 0.002;
		//if (mpbAllowSeveranceForShorterPeriod->IsChecked() && mnYears >= partYearThreshold)
		if (mpbAllowSeveranceForShorterPeriod->IsChecked() && gWorkYears.mbAllowPartYearSeverance)
		{
			mDuePay = mPayPerYear * mnYears;
			msDue += L"Pay for ";
			msDue += ToString(mnYears);
			msDue += L" year * ";
			msDue += ToString(mPayPerYear);
			msDue += L" ==> ";
			msDue += ToString(mDuePay);
		}
		else
		{
			mDuePay = 0;
			msDue += L"None - Period too short";
			return true;
		}
	}
	else
	{
		mDuePay = (int)(mnYears * mPayPerYear + 0.5);

		msDue += L"Pay for ";
		msDue += ToString(mnYears ,3);
		msDue += L" years * ";
		msDue += ToString(mPayPerYear);
		msDue += L" ==> ";
		msDue += ToString(mDuePay);
	}

	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		mDuePay = mDuePay * gFamilyPart.GetRatio();
		msDue += L" =FamilyPart=> ";
		msDue += ToString(mDuePay);
	}

	if (gWorkYears.mnDaysForSeveranceAddedForUnpaidVacations && gUsedVacations.mbAdd14DaysUnpaidVacation4Severance)
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
	CString s;
	s += ToString(mnYears, 3);
	s += L" years * ";
	s += ToString(mPayPerYear);

	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		s += ", Family Part ";
		s += gFamilyPart.GetSRatio();
	}

	return s;
}
CString CSeverance::GetDecriptionForLetterHebrew(void)
{
	CString s;

	s += ToString(mnYears, 3);
	s += L" שנים * ";
	s += ToString(mPayPerYear);

	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		s += L", חלק המשפחה ";
		s += gFamilyPart.GetSRatio();
	}
	return s;
}
