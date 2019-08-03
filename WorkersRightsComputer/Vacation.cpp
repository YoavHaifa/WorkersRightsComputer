#include "StdAfx.h"
#include "Vacation.h"
#include "Utils.h"
#include "VacationTable.h"

const double CVacation::MIN_FRACTION_FOR_ROUND_UP = 0.9;


CVacation::CVacation(void)
	: CRight(L"Vacation", L"חופש")
	, mPayPerDay(0)
{
	miPrintOrder = 2;
}
bool CVacation::SetCheckRef(CButtonRef *pButton)
{
	if (pButton->msName == L"DemandVac4Prev")
	{
		mpbDemandForPreviousYears = pButton;
		return true;
	}
	return false;
}
bool CVacation::SetEditRef(class CEditRef *pRef)
{
	if (pRef->msName == L"VacationPrevYears")
	{
		mpPrevYearsBox = &pRef->mEdit;
		return true;
	}
	return false;
}
bool CVacation::ComputePerPeriod(void)
{
	double restMonths = mnYearsForVacation * 12;
	int seniority = mnSeniority;
	int nMonthInSeniorityYear = 12;
	double daysFraction = 0;

	if (gWorkPeriod.mnMonths < N_MONTH_FOR_FULL_YEARLY_VACATION)
	{
		restMonths += gWorkPeriod.mnMonths;
		seniority ++;
		nMonthInSeniorityYear = gWorkPeriod.mnMonths;

		daysFraction = (double)gWorkPeriod.mnDays / 30;
		if (daysFraction > 1)
			daysFraction = 1;
	}
		
	mCurYear = gWorkPeriod.mLast.mYear;
	mCurMonth = gWorkPeriod.mLast.mMonth;
	LogLine(L"mnYearsForVacation", mnYearsForVacation);
	LogLine(L"seniority", seniority);
	LogLine(L"Year", mCurYear);
	int nMonthForYear = 0;
	double sumYear = 0;

	while (restMonths > 0)
	{
		double nDaysPerMonth = gVacationTable.GetNDaysPerMonth(seniority, gWorkPeriod.mnWorkDaysPerWeek, mCurYear, mCurMonth);
		LogLine(L"Month ", mCurMonth, L" due days", nDaysPerMonth);
		sumYear += nDaysPerMonth;
		CountBackMonth();

		if (restMonths >= 1)
		{
			mnDueDays += nDaysPerMonth;
			restMonths -= 1;

			if (daysFraction > 0)
			{
				LogLine(L"daysFraction ", daysFraction);
				double daysFractionVacation = nDaysPerMonth * daysFraction;
				LogLine(L"daysFractionVacation ", daysFractionVacation);
				mnDueDays += daysFractionVacation;
				sumYear += daysFractionVacation;
				daysFraction = 0;
			}
		}
		else
		{
			double daysPerPartPartMonth = nDaysPerMonth * restMonths;
			LogLine(L"days due per part month", daysPerPartPartMonth);
			mnDueDays += daysPerPartPartMonth;
			restMonths = 0;
		}
		nMonthForYear++;
		if (nMonthForYear == nMonthInSeniorityYear)
		{
			nMonthForYear = 0;
			seniority--;
			LogLine(L"==== days per year", sumYear);
			sumYear = 0;
			nMonthInSeniorityYear = 12;
			LogLine(L"seniority", seniority);
		}
	}

	// Compute Months
	/*
	if (gWorkPeriod.mnMonths < N_MONTH_FOR_FULL_YEARLY_VACATION)
	{
		int daysPerLastMonth = GetNDaysPerMonth(gWorkPeriod.mnYears + 1, gWorkPeriod.mnWorkDaysPerWeek, mCurYear, mCurMonth);
		LogLine(L"n days for full last year", daysPerLastMonth);
		mnMonthsForVacation = gWorkPeriod.mnMonths;
		double fraction = (double)gWorkPeriod.mnDays / 30;
		if (fraction > 1)
			fraction = 1;
		mnMonthsForVacation += fraction;

		double vacFrac = mnMonthsForVacation * daysPerLastYear / 12;
		LogLine(L"days for last part year with fraction", vacFrac);
		mnDueDays += vacFrac;
	} */

	// Round Up or Down
	LogLine(L"n due days due with fraction", mnDueDays);
	int nFullDays = (int)mnDueDays;
	double dayFrac = mnDueDays - nFullDays;
	if (dayFrac >= MIN_FRACTION_FOR_ROUND_UP)
	{
		nFullDays++;
		LogLine(L"n full days rounded up", nFullDays);
	}
	else
		LogLine(L"n full days rounded down", nFullDays);
	mnDueDays = nFullDays;

	LogLine(L"n due days final", mnDueDays);

	mDuePay = (int)(mnDueDays * mPayPerDay + 0.5);
	msDesc += ToString(mnDueDays);
	msDescHebrew  += ToString(mnDueDays);
	msDesc += L" days * ";
	msDescHebrew += L" ימים * ";
	msDesc += ToString(mPayPerDay);
	msDescHebrew += ToString(mPayPerDay);
	return true;
}
bool CVacation::Compute(void)
{
	LogLine(L"N work days per week", gWorkPeriod.mnWorkDaysPerWeek);
	if (!mpbDemandForPreviousYears->IsChecked())
	{
		mnYearsOfUnpaidVacation = 0;
		mpPrevYearsBox->SetWindowTextW(L"0");
	}

	// Init
	mnSeniority = 0;
	mnDueDays = 0;
	mbUseShortRule = false;
	mnYearsForVacation = 0;
	mnMonthsForVacation = 0;
	msDesc = L"";
	msDescHebrew = L"";

	if (!gWorkPeriod.IsValid())
	{
		msDue += L"Define Work Period...";
		return false;
	}
	LogLine(L"previous work years", gWorkPeriod.mnFullWorkYears);

	// Check for request to previous years
	mnYearsOfUnpaidVacation = 0;
	CString sText;
	mpPrevYearsBox->GetWindowTextW(sText);
	if ((sText.GetLength() > 0) && mpbDemandForPreviousYears->IsChecked())
	{
		mnYearsOfUnpaidVacation = _wtof(sText);
		LogLine(L"Demand N Prev Years to Pay Vacation", mnYearsOfUnpaidVacation);
		if (mnYearsOfUnpaidVacation > gWorkPeriod.mnFullWorkYears)
		{
			mnYearsOfUnpaidVacation = gWorkPeriod.mnFullWorkYears;
			mpPrevYearsBox->SetWindowTextW(ToString(mnYearsOfUnpaidVacation));
			LogLine(L"N Prev Years to Pay cut to n worked", mnYearsOfUnpaidVacation);
		}
		if (mnYearsOfUnpaidVacation > MAX_FULL_YEARS)
		{
			mnYearsOfUnpaidVacation = MAX_FULL_YEARS;
			mpPrevYearsBox->SetWindowTextW(ToString(mnYearsOfUnpaidVacation));
			LogLine(L"N Prev Years to Pay cut to MAX allowed", mnYearsOfUnpaidVacation);
		}
	}

	mPayPerDay = gMinWage.PayPerDayAtWorkEnd();
	LogLine(L"Pay per day at work end", mPayPerDay);

	if (gVacationTable.mn < 1)
	{
		msDue += L"Missing Vacation's File!";
		return false;
	}

	if (gWorkPeriod.mnWorkingDays < MIN_DAYS_FOR_SHORT_RULE)
	{
		mbUseShortRule = true;
		mDuePay = gWorkPeriod.mnWorkingDays * mPayPerDay * SHORT_RULE_PER_CENT / 100;
		msDebug += L" Special rule, less than ";
		msDebug += ToString(MIN_DAYS_FOR_SHORT_RULE);
		msDebug += L" days";
		LogLine(L"Short period: n working days", gWorkPeriod.mnWorkingDays);
		LogLine(L"pay per day", mPayPerDay);
		LogLine(L"per cent", SHORT_RULE_PER_CENT);
		msDue += L"(Short period last year) ";
		msDesc += L"Short Period: ";
		msDescHebrew += L"תקופה קצרה: ";
		msDesc += ToString(gWorkPeriod.mnWorkingDays);
		msDescHebrew += ToString(gWorkPeriod.mnWorkingDays);
		msDesc += L" days * ";
		msDescHebrew += L" ימים * ";
		msDesc += ToString(mPayPerDay);
		msDescHebrew += ToString(mPayPerDay);
		msDesc += L" * ";
		msDescHebrew += L" * ";
		msDesc += ToString(SHORT_RULE_PER_CENT);
		msDescHebrew += ToString(SHORT_RULE_PER_CENT);
		msDesc += L"%";
		msDescHebrew += L"%";
	}
	else
	{
		mnSeniority = gWorkPeriod.mnFullWorkYears;
		mnYearsForVacation = mnYearsOfUnpaidVacation;
		LogLine(L"N Previous Years for vacation", mnYearsForVacation);
		if (gWorkPeriod.mnMonths >= N_MONTH_FOR_FULL_YEARLY_VACATION)
		{
			mnYearsForVacation++;
			mnSeniority++;
			LogLine(L"Last Year Fraction is regarded full year, n full years =", mnYearsForVacation);
		}

		if (!ComputePerPeriod())
			return false;
	}
	PrepareString();
	return true;
}
void CVacation::PrepareString(void)
{
	if (mbUseShortRule)
	{
		msDue += ToString(SHORT_RULE_PER_CENT);
		msDue += L"% of bruto";
	}
	else
	{
		msDue += L" (";
		if (mnYearsForVacation)
		{
			msDue += ToString(mnYearsForVacation, 3);
			msDue += L" years";
		}
		if (mnMonthsForVacation > 0)
		{
			if (mnYearsForVacation)
				msDue += L", ";
			msDue += ToString(mnMonthsForVacation);
			msDue += L" months";
		}
		msDue += L")  ";

		msDue += ToString(mnDueDays);
		msDue += L" Days * ";
		msDue += ToString(mPayPerDay);
	}

	msDue += L" ==> ";
	msDue += ToString(mDuePay);
	//msDue += L" NIS";
}
CString CVacation::GetDecriptionForLetter(void)
{
	return msDesc;
}
CString CVacation::GetDecriptionForLetterHebrew(void)
{
	return msDescHebrew;
}
void CVacation::CountBackMonth(void)
{
	if (mCurMonth <= 1)
	{
		mCurMonth = 12;
		mCurYear--;
	}
	else
		mCurMonth--;
}
