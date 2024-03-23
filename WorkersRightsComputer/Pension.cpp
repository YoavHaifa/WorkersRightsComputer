#include "StdAfx.h"
#include "Pension.h"
#include "MonthlyRates.h"
#include "Utils.h"
#include "MonthInfo.h"
#include "HtmlWriter.h"
#include "FamilyPart.h"
#include "WorkYears.h"
#include "UsedVacations.h"
#include "WageTable.h"
#include "WorkPeriod.h"
#include "MonthlyFamilyPart.h"

CPension *gpPension = NULL;

CPension::CPension(void)
	: CRight(L"Pension", L"פנסיה")
	, mbEntitledOnlyToSeveranceFund(false)
	, mbHadActivePensionBefore(false)
{
	miPrintOrder = 0;
	mpPensionRates = new CMonthlyRates(L"Pension", 2008); // Load from XML
	mpSeveranceMonthlyRates = new CMonthlyRates(L"Severance", 2008);
	gpPension = this;
}
CPension::~CPension(void)
{
	gpPension = NULL;
}
bool CPension::SetCheckRef(CButtonRef *pButton)
{
	if (pButton->msName == L"EntitledOnly2Sev")
	{
		mpbEntitledOnlyToSeveranceFund = pButton;
		return true;
	}
	if (pButton->msName == L"HadPensionBefore")
	{
		mpbHadActivePensionBefore = pButton;
		return true;
	}
	return false;
}
bool CPension::Compute(void)
{
	mbSeverance = false;
	gWorkPeriod.Log(L"Pension_Compute");

	if (!mpbEntitledOnlyToSeveranceFund)
	{
		CUtils::MessBox(L"Connection to GUI not initialized", L"SW Error");
		return false;
	}
	mbEntitledOnlyToSeveranceFund = mpbEntitledOnlyToSeveranceFund->IsChecked();
	mbHadActivePensionBefore = mpbHadActivePensionBefore->IsChecked();

	mDuePay = 0;
	mSeveranceDue = 0;
	mPensionDue = 0;
	mDueFromFamily = 0;
	mPensionPerYear = 0;
	mSeverancePerYear = 0;

	if (mbEntitledOnlyToSeveranceFund)
		LogLine (L"+++ Entitled ONLY To Severance Fund");
	else
		LogLine(L"--- Not Entitled ONLY To Severance Fund");

	if (mbHadActivePensionBefore)
		LogLine(L"+++ Had Active Pension Before");
	else
		LogLine(L"--- No Active Pension Before");

	LogLine(L"");
	LogLine(L"Started work at", gWorkPeriod.mFirst.ToString());
	LogLine(L"Last day at work", gWorkPeriod.mLast.ToString());
	LogLine(L"");

	bool bOK = DoCompute();

	if (mbEntitledOnlyToSeveranceFund)
	{
		mbSeverance = true;
		LogLine(L"Pension Due", mPensionDue);
		LogLine(L"Severance Due", mSeveranceDue);

		msDue += L" Total Due ";
	}
	else
		msDue += L" Due ";
	msDue += ToString(mDuePay);
	return bOK;
}
void CPension::PensionAddMonth(CMyTime& date, int nDays /* if 0 - full */, bool bFirst)
{
	//int year = yearAndMonth.mYear;
	//int month = yearAndMonth.mMonth;
	double part = 1;
	CMonthInfo* pMonthInfo = gWorkPeriod.GetMonthInfoFor(date);
	double monthFraction = pMonthInfo->GetFractionForPension();
	CMonthlyFamilyPart monthlyFamilyPart(*pMonthInfo);

	if (bFirst)
	{
		if (nDays > 0)
		{
			part = (double)nDays / 30.0;
			if (part > 1)
				part = 1;
		}
		if (bFirst)
			part = min(part, monthFraction);
	}
	if (part == 1)
	{
		part = monthFraction;
	}

	double monthlyPay = gWageTable.ComputeMonthlyPay(date);

	double penRate = mpPensionRates->RatePerMonth(date);
	double pensionDue = monthlyPay * penRate * part;

	double familyPart = 0;
	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		familyPart = monthlyFamilyPart.mFamilyRatio;
		pensionDue *= familyPart;
	}
	mPensionDue += pensionDue;
	mPensionPerYear += pensionDue;

	CString sLine = ToString(date.mYear);
	sLine += L" ";
	sLine += ToString(date.mMonth);
	sLine += L": ";
	sLine += ToString(part, 3);
	sLine += L" ";
	sLine += ToString(monthlyPay);
	sLine += L" - ";
	sLine += ToString(penRate, 5);
	sLine += L" ";
	sLine += ToString(pensionDue);

	double severanceDue = 0;
	double sevRate = 0;
	if (mbEntitledOnlyToSeveranceFund)
	{
		sevRate = mpSeveranceMonthlyRates->RatePerMonth(date);
		severanceDue = monthlyPay * sevRate * part;
		if (gFamilyPart.mbAskOnlyForFamilyPart)
			severanceDue *= familyPart;
		mSeveranceDue += severanceDue;
		mSeverancePerYear += severanceDue;
		double all = severanceDue + pensionDue;

		sLine += L" - ";
		sLine += ToString(sevRate, 5);
		sLine += L" ";
		sLine += ToString(severanceDue);
		sLine += L" = ";
		sLine += ToString(all);
	}
	LogLine(sLine);
	if (date.mMonth == 12)
		OnYearEnd();
	if (gFamilyPart.mbAskOnlyForFamilyPart)
		mDueFromFamily += (pensionDue + severanceDue);
	mReport.AddMonth(date, monthlyPay, part, penRate, sevRate, familyPart, (const wchar_t *)monthlyFamilyPart.msCompanyRatio);
}
bool CPension::CheckForDateToStartPension()
{
	if (mbHadActivePensionBefore)
	{
		if (!gWorkYears.WorkedAtLeastNMonths(N_MONTHS_BEFORE_PAY_CONTINUITY))
		{
			msDue += L"Period Too Short";
			msDebug += L"Too short even for continuity pay";
			return false;
		}
		mStartDateForPension = gWorkPeriod.mFirst;
		msDebug += L"Continuity ";
	}
	else
	{
		CMyTime startDateForPension8;
		startDateForPension8.AddMonths(gWorkPeriod.mFirst, N_MONTHS_BEFORE_PAY_FROM_2008);
		CMyTime startDateForPension9;
		startDateForPension9.AddMonths(gWorkPeriod.mFirst, N_MONTHS_BEFORE_PAY_FROM_2009);
		if (startDateForPension8.mYear < 2009)
			mStartDateForPension = startDateForPension8;
		else if (startDateForPension9.mYear > 2008)
			mStartDateForPension = startDateForPension9;
		else
			mStartDateForPension.Set(2009, 1, 1);

		if (mStartDateForPension > gWorkPeriod.mLast)
		{
			msDue += L"Period Too Short";
			msDebug += L"Too short for pension pay";
			return false;
		}
	}

	if (mStartDateForPension.mYear < YEAR_TO_START)
	{
		mStartDateForPension.Set(YEAR_TO_START, 1, 1);
		msDebug += L" Period before 2008 skipped";
	}
	if (mStartDateForPension.mYear >= YEAR_TO_START_CHECKING_VACATIONS)
		UpdateStartDateForPensionForVacations();

	LogLine(L"First day for pension", mStartDateForPension);
	LogLine(L"");
	return true;
}
bool CPension::DoCompute()
{
	mReport.Clear();

	if (!CheckForDateToStartPension())
		return true; // Period too short for any payment

	// Compute first part-month
	bool bFirstIsLast = false;
	int nDaysInFirstMonth = gWorkPeriod.CountDaysToEndOfMonth(mStartDateForPension);
	if (mStartDateForPension.mYear == gWorkPeriod.mLast.mYear 
		&& mStartDateForPension.mMonth == gWorkPeriod.mLast.mMonth)
	{
		nDaysInFirstMonth = gWorkPeriod.mLast.mDay - mStartDateForPension.mDay + 1;
		bFirstIsLast = true;
		if (nDaysInFirstMonth < 1)
		{
			msDue += L"Period Too Short";
			msDebug += L"Too short for pension pay";
			return true;
		}
	}
	// Add first part-month
	CMonthlyFamilyPart::StartComputingPension();
	PensionAddMonth(mStartDateForPension, nDaysInFirstMonth, true);

	if (!bFirstIsLast)
	{
		CMyTime countMonths(mStartDateForPension.mYear, mStartDateForPension.mMonth, 1);
		countMonths.AddMonth();
		while ((countMonths.mYear < gWorkPeriod.mLast.mYear)
			|| (countMonths.mYear == gWorkPeriod.mLast.mYear &&  countMonths.mMonth < gWorkPeriod.mLast.mMonth))
		{
			// Add regular full month
			PensionAddMonth(countMonths, 0);
			countMonths.AddMonth();
		}

		// Compute Last Month
		PensionAddMonth(countMonths, gWorkPeriod.mLast.mDay);
		if (countMonths.mMonth < 12)
			OnYearEnd();
	}

	CMonthlyFamilyPart::EndComputingPension();
	mDuePay = mSeveranceDue + mPensionDue;
	return true;
}
void CPension::OnYearEnd(void)
{
	CString s = L"Total per year: Pension ";
	s += ToString(mPensionPerYear);
	if (mbEntitledOnlyToSeveranceFund)
	{
		s += L", Severance ";
		s += ToString(mSeverancePerYear);

		double all = mPensionPerYear + mSeverancePerYear;
		s += L" = ";
		s += ToString(all);
	}
	LogLine(s);
	LogLine(L"");

	mPensionPerYear = 0;
	mSeverancePerYear = 0;
}
CString CPension::GetDecriptionForLetter(void)
{
	CString s;
	if (gFamilyPart.mbAskOnlyForFamilyPart)
		s += "Family Part, ";

	if (mSeveranceDue > 0)
	{
		s += L"Severance ";
		s += ToString(mSeveranceDue);
		s += L" + Pension ";
		s += ToString(mPensionDue);
	}
	else
	{
		s += L"Only Pension";
	}
	s += " (see appendix)";
	return s;
}
CString CPension::GetDecriptionForLetterHebrew(void)
{
	CString s;
	if (gFamilyPart.mbAskOnlyForFamilyPart)
		s += L"חלק המשפחה, ";

	if (mSeveranceDue > 0)
	{
		s += L"קרן פיצויים ";
		s += ToString(mSeveranceDue);
		s += L" + תגמולים ";
		s += ToString(mPensionDue);
	}
	else
	{
		s += L"תגמולים בלבד ";
	}
	s += L"(פרוט בנספח)";

	return s;
}
void CPension::WriteToLetter(class CHtmlWriter& html)
{
	if (mReport.IsEmpty())
		return;

	html.StartParagraph();
	html.WriteLineEH(L"Computation of due pension by month:", 
		L"חישוב זכאות לפנסיה לפי חודשים:");

	if (gFamilyPart.mbAskOnlyForFamilyPart)
		html.WriteLineEH(
			L"The family's share is the number of hours for a full-time job (43 until March 31, 2018, and 42 from then on) minus the number of hours paid by the company.", 
			L"חלק המשפחה בהעסקה הוא מס' שעות למשרה מלאה (43 עד סוף מרץ 2018 ו-42 מאז ואילך) פחות מספר השעות ששילמה החברה.");

	html.WriteEH(L"First day for pension: ", L"חישוב הפנסיה החל מתאריך: ");
	html.WriteLineEH(mStartDateForPension.ToString(), mStartDateForPension.ToHebrewString());

	html.StartBold();
	html.WriteLineEH(L"Pension - Gemel", L"פנסיה - גמל");
	html.EndBold();

	mReport.WriteToLetterPension(html);
	html.EndParagraph();

	if (gpPension->mbSeverance)
	{
		html.StartParagraph();
		html.StartBold();
		html.WriteLineEH(L"Severance Fund", L"קרן פיצויים");
		html.EndBold();
		mReport.WriteToLetterSeverance(html);
		html.EndParagraph();
	}
}
bool CPension::UpdateStartDateForPensionForVacations()
{
	if (mStartDateForPension <= gWorkPeriod.mFirst)
		return false;

	CWorkSpan workBeforePension;
	workBeforePension.InitDaysSpan(gWorkPeriod.mFirst, mStartDateForPension.PrevDay());
	gUsedVacations.AddAllVacationsToWorkSpan(workBeforePension, true /*bExtendPeriodByPaidMaternity*/);

	if (workBeforePension.mDayAfter > mStartDateForPension)
	{
		mStartDateForPension = workBeforePension.mDayAfter;
		LogLine(L"Start day for pension updated due to vacations and/or maternity", mStartDateForPension);
		return true;
	}
	return false;
}
