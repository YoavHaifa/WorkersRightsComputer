#include "StdAfx.h"
#include "Recuperation.h"
#include "WorkPeriod.h"
#include "WorkYears.h"


CRecuperation::CRecuperation(void)
	: CRight(L"Recuperation", L"דמי הבראה")
	, mpbDemandPreviousYears(NULL)
{
	miPrintOrder = 3;
	Init();
	mpSeniority = new CSeniority(L"Recuperation");
	mpRates = new CYearlyRates(L"Recuperation", 1950);
}
bool CRecuperation::SetCheckRef(CButtonRef *pButton)
{
	if (pButton->msName == L"DemandRec4Prev")
	{
		mpbDemandPreviousYears = pButton;
		return true;
	}
	return false;
}
bool CRecuperation::SetEditRef(class CEditRef *pRef)
{
	if (pRef->msName == L"RecuperationPrevYears")
	{
		mpPrevYearsBox = &pRef->mEdit;
		return true;
	}
	return false;
}

bool CRecuperation::Compute(void)
{
	mDuePay = 0;
	msDue = L"Recuperation: ";
	mDueDays = 0;

	if (!gWorkPeriod.IsValid())
	{
		mbValid = false;
		msDue += L"??";
		msDebug += L"Need to define Work Period";
		return false;
	}

	if (gWorkYears.mnFullWorkYears < MIN_YEARS_TO_START)
	{
		mbValid = false;
		msDue += L"0";
		msDebug += L"Work Period too short";
		return false;
	}

	double lastYearFraction = gWorkYears.GetLastYearAsFraction();
	LogLine(L"Last Year Fraction", lastYearFraction);

	int intDaysPerYear = mpSeniority->ma[gWorkYears.mn];
	mDueDays = lastYearFraction * intDaysPerYear;
	LogLine(L"N Due Days for last year", mDueDays);

	int rateYear = gWorkPeriod.mLast.mYear;
	if(gWorkPeriod.mLast.mMonth < MONTH_OF_NEW_RATE)
		rateYear--;
	mRate = mpRates->RatePerYear(rateYear);

	mnYearsBack = 0;
	CString sText;
	mpPrevYearsBox->GetWindowTextW(sText);
	if (sText.GetLength() > 0)
		mnYearsBack = _wtof(sText);
	LogLine(L"N Years Back", mnYearsBack);
	if (mnYearsBack > gWorkYears.mnPrevYears)
	{
		mnYearsBack = gWorkYears.mnPrevYears;
		LogLine(L"N Years Back can't be more than worked", mnYearsBack);
	}

	if (mpbDemandPreviousYears->IsChecked() && mnYearsBack > 0)
	{
		int seniority = gWorkYears.mnPrevYears;
		double rest = mnYearsBack;
		while (rest > 0)
		{
			LogLine(L"Seniority", seniority);
			double daysPerYear = mpSeniority->ma[seniority];
			if (rest >= 1)
			{
				mDueDays += daysPerYear;
				rest -= 1;
				LogLine(L"Add for year", daysPerYear);
			}
			else
			{
				LogLine(L"part of year", rest);
				LogLine(L"days per year", daysPerYear);
				LogLine(L"Add days per part year", daysPerYear * rest);
				mDueDays += daysPerYear * rest;
				rest = 0;
			}

			seniority--;
		}
	}
	LogLine(L"N Due Days Final", mDueDays);
	LogLine(L"Pay per day", mRate);

	mDuePay = mDueDays * mRate;
	mbValid = true;

	msDebug += L" partYear ";
	msDebug += ToString(lastYearFraction);
	if (mpbDemandPreviousYears->IsChecked() && mnYearsBack > 0)
	{
		msDebug += L" mnYearsBack ";
		msDebug += ToString(mnYearsBack);
	}
	msDebug += L" mDueDays ";
	msDebug += ToString(mDueDays);
	msDebug += L" mRate ";
	msDebug += ToString(mRate);
	msDebug += L" mDuePay ";
	msDebug += ToString(mDuePay);

	// Due String
	msDue += ToString(mDueDays);
	msDue += L" Days * ";
	msDue += ToString(mRate);
	msDue += L" ==> ";
	msDue += ToString(mDuePay);

	return true;
}
CString CRecuperation::GetDecriptionForLetter(void)
{
	CString s = ToString(mDueDays);
	s += L" days * ";
	s += ToString(mRate);

	return s;
}
CString CRecuperation::GetDecriptionForLetterHebrew(void)
{
	CString s = ToString(mDueDays);
	s += L" ימים * ";
	s += ToString(mRate);

	return s;
}
