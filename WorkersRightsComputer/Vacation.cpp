#include "StdAfx.h"
#include "Vacation.h"
#include "Utils.h"
#include "VacationTable.h"
#include "WorkYears.h"
#include "Config.h"
#include "WageTable.h"

const double CVacation::MIN_FRACTION_FOR_ROUND_UP = 0.9;

CVacation::CVacation()
	: CRight(L"Vacation", L"חופש")
	, mPayPerDay(0)
{
	miPrintOrder = 2;
	//gVacationTable.InitFromTextFile();
	//gVacationTable.SaveXmlFile();
	gVacationTable.InitFromXmlFile();

}
bool CVacation::SetCheckRef(CButtonRef *pButton)
{
	if (pButton->msName == L"DemandVac4Prev")
	{
		mpbDemandForPreviousYears = pButton;
		return true;
	}
	else if (pButton->msName == L"PaidLastYVacation")
	{
		mpbAllPaid4LastYear = pButton;
		return true;
	}
	else if (pButton->msName == L"PaidDaysLastYVacation")
	{
		mpbDaysPaid4LastYear = pButton;
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
	else if (pRef->msName == L"VacationDaysPaidLastYear")
	{
		mpLastYearsPaidDaysBox = &pRef->mEdit;
		return true;
	}
	return false;
}
bool CVacation::MissingInput(CString& iosText)
{
	mbLastYearDaysPaid = mpbDaysPaid4LastYear->IsChecked();
	if (!mbLastYearDaysPaid)
		return false;

	if (GetGUIValue(mpLastYearsPaidDaysBox, mnDaysPaidLastYear, mpbDaysPaid4LastYear))
	{
		if (mnDaysPaidLastYear > 0 && mnDaysPaidLastYear < 1000)
			return false;
	}

	iosText += L"Number of paid vacation days is not defined.\r\n";
	return true;
}
bool CVacation::ReducePaidDays()
{
	if (mnDaysPaidLastYear <= 0)
		return true; // Do Nothing

	// Reduce paid days (may be more than only last year)
	if (mnDaysPaidLastYear > mnDueDays)
	{
		LogLine(L"Due vacation days", mnDueDays);
		LogLine(L"Paid vacation days more or equal due days", mnDaysPaidLastYear);
		mnDueDays = 0;
		msDue = "Input Error: Number of paid vacation days more than due days";
		return false;
	}

	if (mnDaysPaidLastYear == mnDueDays)
	{
		LogLine(L"Paid vacation days equal to due days", mnDaysPaidLastYear);
		mnDueDays = 0;
	}
	else
	{
		LogLine(L"Initial due vacation days", mnDueDays);
		LogLine(L"Subtract paid vacation days", mnDaysPaidLastYear);
		mnDueDays -= mnDaysPaidLastYear;
	}
	return true;
}
bool CVacation::GetGUIValue(CEdit *pEdit, double &oValue, CButtonRef *pButton)
{
	oValue = 0;
	if (pButton)
	{
		if (!pButton->IsChecked())
			return false;
	}

	CString sText;
	pEdit->GetWindowTextW(sText);
	if (sText.GetLength() < 1)
		return false;

	oValue = _wtof(sText);
	return true;
}
void CVacation::GetNPrevYears()
{
	LogLine(L"previous work years", gWorkYears.mnPrevYears);

	// Check for request to previous years
	if (GetGUIValue(mpPrevYearsBox, mnYearsOfUnpaidVacation, mpbDemandForPreviousYears))
	{
		LogLine(L"Demand N Prev Years to Pay Vacation", mnYearsOfUnpaidVacation);
		if (mnYearsOfUnpaidVacation > gWorkYears.mnPrevYears)
		{
			mnYearsOfUnpaidVacation = gWorkYears.mnPrevYears;
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
}
bool CVacation::ComputeByShortRule()
{
	mbUseShortRule = true;
	mnDueDays = gWorkYears.mnWorkingDays * SHORT_RULE_PER_CENT / 100;
	if (!ReducePaidDays())
		return false;

	mDuePay = mnDueDays * mPayPerDay;
	msDebug += L" Special rule, less than ";
	msDebug += ToString(MIN_DAYS_FOR_SHORT_RULE);
	msDebug += L" days";
	LogLine(L"Short period: n working days", gWorkYears.mnWorkingDays);
	LogLine(L"pay per day", mPayPerDay);
	LogLine(L"per cent", SHORT_RULE_PER_CENT);
	msDue += L"(Short period last year) ";
	msDesc += L"Short Period: ";
	msDescHebrew += L"תקופה קצרה: ";
	msDesc += ToString(gWorkYears.mnWorkingDays);
	msDescHebrew += ToString(gWorkYears.mnWorkingDays);
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
	return true;
}
bool CVacation::ComputeInternals()
{
	if (mbLastYearAllPaid)
	{
		msDebug += L"Vacation All Paid";
		LogLine(L"Vacation All Paid");
		return true;
	}

	if (gWorkYears.mnWorkingDays < MIN_DAYS_FOR_SHORT_RULE)
		return ComputeByShortRule();

	mnSeniority = gWorkYears.mnPrevYears;
	mnYearsForVacation = mnYearsOfUnpaidVacation;
	LogLine(L"N Previous Years for vacation", mnYearsForVacation);
	
	// Add last year
	mnMonthsForVacation = mnYearsForVacation * 12 + gWorkYears.mnMonthsInLastYear;
	mnSeniority++;
	LogLine(L"N months for vacation", mnMonthsForVacation);

	ComputeBackByMonth();
	ComputeBackByYears();

	if (!ReducePaidDays())
		return false;

	ComputeEpilogue();
	return true;
}
void CVacation::ComputeEpilogue()
{
	// Round Up or Down
	LogLine(L"n due days with fraction", mnDueDays);
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
	msDescHebrew += ToString(mnDueDays);
	msDesc += L" days * ";
	msDescHebrew += L" ימים * ";
	msDesc += ToString(mPayPerDay);
	msDescHebrew += ToString(mPayPerDay);
}
void CVacation::ComputeBackByYears()
{
	LogLine(L"<ComputeBackByYears>");
	mnSeniority = gWorkYears.mnPrevYears + 1;
	mnYearsForVacation = mnYearsOfUnpaidVacation + 1;
	LogLine(L"n years for vacation", mnYearsForVacation);

	// Compute last year first
	mnDueDays = 0;
	for (int iFromLast = 0; iFromLast < mnYearsForVacation; iFromLast++)
	{
		CWorkYear *pYear = gWorkYears.GetByReverseIndex(iFromLast);
		double vacationFraction = pYear->GetVacationFraction();
		double nDaysPerYear = gVacationTable.GetNDaysPerYear(pYear->mSeniority, gWorkPeriod.mnWorkDaysPerWeek);
		double nDueDaysPerYear = nDaysPerYear * vacationFraction;
		mnDueDays += nDueDaysPerYear;

		// Log computations
		LogLine(L"Work Year", gWorkYears.mn - iFromLast);
		LogLine(L"Seniority", pYear->mSeniority);
		LogLine(L"Days due per year, by seniority", nDaysPerYear);
		LogLine(L"VacationFraction", vacationFraction);
		LogLine(L"Final nDueDaysPerYear", nDueDaysPerYear);
		LogLine(L"==== Accumulated Vacation Days", mnDueDays);
	}
}
void CVacation::ComputeBackByMonth()
{
	double restMonths = mnMonthsForVacation;
	int seniority = mnSeniority;
	int nMonthInSeniorityYear = gWorkYears.mnMonthsInLastYear;
	double daysFraction = gWorkYears.mDaysInLastYearAsFraction;

	mCurYear = gWorkPeriod.mLast.mYear;
	mCurMonth = gWorkPeriod.mLast.mMonth;
	LogLine(L"Year", mCurYear);
	int nMonthForYear = 0;
	double sumYear = 0;

	while (restMonths > 0)
	{
		LogLine(L"seniority", seniority);

		double nDaysPerMonth = gVacationTable.GetNDaysPerMonth(seniority,
			gWorkPeriod.mnWorkDaysPerWeek, mCurYear, mCurMonth);
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
		}
	}

	LogLine(L"<ComputeBackByMonth> Accumulated Vacation Days", mnDueDays);
}
bool CVacation::Compute()
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
	mbLastYearAllPaid = mpbAllPaid4LastYear->IsChecked();
	mbLastYearDaysPaid = mpbDaysPaid4LastYear->IsChecked();
	mnYearsForVacation = 0;
	mnMonthsForVacation = 0;
	mnDaysPaidLastYear = 0;
	msDesc = L"";
	msDescHebrew = L"";

	if (!gWorkPeriod.IsValid())
	{
		msDue += L"Define Work Period...";
		return false;
	}

	if (GetGUIValue(mpLastYearsPaidDaysBox, mnDaysPaidLastYear, mpbDaysPaid4LastYear))
	{
		if (mnDaysPaidLastYear <= 0 && mnDaysPaidLastYear > 1000)
		{
			msDue += L"Define number of paid vacation days...";
			return false;
		}
	}

	GetNPrevYears();

	mPayPerDay = gWageTable.PayPerDayAtWorkEnd();
	LogLine(L"Pay per day at work end", mPayPerDay);

	if (gVacationTable.mn < 1)
	{
		msDue += L"Missing Vacation's Seniority Definitions!";
		return false;
	}

	if (!ComputeInternals())
		return false;

	PrepareString();
	return true;
}
void CVacation::PrepareString()
{
	if (mbUseShortRule)
	{
		msDue += ToString(SHORT_RULE_PER_CENT);
		msDue += L"% of bruto";
	}
	else
	{
		if (mnYearsForVacation > 0 || mnMonthsForVacation > 0)
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
		}

		msDue += ToString(mnDueDays);
		msDue += L" Days * ";
		msDue += ToString(mPayPerDay);
	}

	msDue += L" ==> ";
	msDue += ToString(mDuePay);
	//msDue += L" NIS";
}
CString CVacation::GetDecriptionForLetter()
{
	return msDesc;
}
CString CVacation::GetDecriptionForLetterHebrew()
{
	return msDescHebrew;
}
void CVacation::CountBackMonth()
{
	if (mCurMonth <= 1)
	{
		mCurMonth = 12;
		mCurYear--;
	}
	else
		mCurMonth--;
}
