#include "StdAfx.h"
#include "Pension.h"
#include "YearlyRates.h"
#include "MonthlyRates.h"
#include "Utils.h"
#include "MonthInfo.h"
#include "HtmlWriter.h"
#include "FamilyPart.h"

CPension *gpPension = NULL;

CPension::CPension(void)
	: CRight(L"Pension", L"פנסיה")
{
	miPrintOrder = 0;
	mpPensionRates = new CMonthlyRates(L"Pension", 2008);
	mpSeveranceRates = new CYearlyRates(L"Severance", 2008);
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
		mpbEntitledToSeveranceFund = pButton;
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

	if (!mpbEntitledToSeveranceFund)
	{
		CUtils::MessBox(L"Connection to GUI not initialized", L"SW Error");
		return false;
	}
	mDuePay = 0;
	mSeveranceDue = 0;
	mPensionDue = 0;
	mDueFromFamily = 0;
	mPensionPerYear = 0;
	mSeverancePerYear = 0;

	if (mpbEntitledToSeveranceFund->IsChecked())
		LogLine (L"+++ Entitled To Severance Fund");
	else
		LogLine(L"--- Not Entitled To Severance Fund");

	if (mpbHadActivePensionBefore->IsChecked())
		LogLine(L"+++ Had Active Pension Before");
	else
		LogLine(L"--- No Active Pension Before");

	LogLine(L"");
	CString s = L"Started work at ";
	s += gWorkPeriod.mFirst.ToString();
	LogLine(s);
	s = L"Last day at work ";
	s += gWorkPeriod.mLast.ToString();
	LogLine(s);
	LogLine(L"");

	bool bOK = DoCompute();

	if (mpbEntitledToSeveranceFund->IsChecked())
	{
		mbSeverance = true;
		CString sLog = L"Pension Due ";
		sLog += ToString(mPensionDue);
		LogLine(sLog);
		sLog = L"Severance Due ";
		sLog += ToString(mSeveranceDue);
		LogLine(sLog);

		msDue += L" Total Due ";
		msDue += ToString(mDuePay);
	}
	else
	{
		msDue += L" Due ";
		msDue += ToString(mDuePay);
	}

	return bOK;
}
void CPension::AddMonth(int year, int month, int nDays /* if 0 - full */)
{
	double part = 1;
	double familyPart = 0;
	if (umbOldStyle)
	{
		if (nDays > 0)
		{
			part = (double)nDays / 30.0;
			if (part > 1)
				part = 1;
		}
	}
	if (part == 1)
	{
		CMonthInfo *pInfo = gWorkPeriod.GetMonthInfoFor(year, month);
		part = pInfo->mFraction;
		if (gFamilyPart.mbAskOnlyForFamilyPart)
			familyPart = 1 - pInfo->GetCompanyRatio();
	}

	double monthlyPay = gMinWage.ComputeMonthlyPay(year, month);

	double penRate = mpPensionRates->RatePerMonth(year, month);
	double pensionDue = monthlyPay * penRate * part;
	if (gFamilyPart.mbAskOnlyForFamilyPart)
		pensionDue *= familyPart;
	mPensionDue += pensionDue;
	mPensionPerYear += pensionDue;

	CString sLine = ToString(year);
	sLine += L" ";
	sLine += ToString(month);
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
	if (mpbEntitledToSeveranceFund->IsChecked())
	{
		sevRate = mpSeveranceRates->RatePerYear(year);
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
	if (month == 12)
		OnYearEnd();
	if (gFamilyPart.mbAskOnlyForFamilyPart)
		mDueFromFamily += (pensionDue + severanceDue);
	mReport.AddMonth(year, month, monthlyPay, part, penRate, sevRate, familyPart);
}
bool CPension::DoCompute()
{
	mReport.Clear();

	if (mpbHadActivePensionBefore->IsChecked())
	{
		if (!gWorkPeriod.WorkedAtLeastNMonths(N_MONTHS_BEFORE_PAY_CONTINUITY))
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
			mStartDateForPension.Set(2009,1,1);

		if (mStartDateForPension > gWorkPeriod.mLast)
		{
			msDue += L"Period Too Short";
			msDebug += L"Too short pension pay";
			return false;
		}
	}

	if (mStartDateForPension.mYear < YEAR_TO_START)
	{
		mStartDateForPension.Set(YEAR_TO_START, 1,  1);
		msDebug += L" Period before 2008 skipped";
	}
	CString sDate = L"First day for pension ";
	sDate += mStartDateForPension.ToString();
	LogLine(sDate);
	LogLine(L"");

	// Compute first part-month
	bool bFirstIsLast = false;
	int nDaysInFirstMonth = gWorkPeriod.CountDaysToEndOfMonth(mStartDateForPension);
	if (mStartDateForPension.mYear == gWorkPeriod.mLast.mYear && mStartDateForPension.mMonth == gWorkPeriod.mLast.mMonth)
	{
		nDaysInFirstMonth = gWorkPeriod.mLast.mDay - mStartDateForPension.mDay + 1;
		bFirstIsLast = true;
	}
	AddMonth(mStartDateForPension.mYear, mStartDateForPension.mMonth, nDaysInFirstMonth);

	if (!bFirstIsLast)
	{
		CMyTime countMonths(mStartDateForPension.mYear, mStartDateForPension.mMonth, 1);
		countMonths.AddMonth();
		while ((countMonths.mYear < gWorkPeriod.mLast.mYear)
			|| (countMonths.mYear == gWorkPeriod.mLast.mYear &&  countMonths.mMonth < gWorkPeriod.mLast.mMonth))
		{
			AddMonth(countMonths.mYear, countMonths.mMonth, 0);
			countMonths.AddMonth();
		}

		// Compute Last Month
		AddMonth(countMonths.mYear, countMonths.mMonth, gWorkPeriod.mLast.mDay);
		if (countMonths.mMonth < 12)
			OnYearEnd();
	}

	mDuePay = mSeveranceDue + mPensionDue;
	return true;
}
void CPension::OnYearEnd(void)
{
	CString s = L"Total per year: Pension ";
	s += ToString(mPensionPerYear);
	if (mpbEntitledToSeveranceFund->IsChecked())
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
	
	return s;
}
CString CPension::GetDecriptionForLetterHebrew(void)
{
	CString s;
	if (gFamilyPart.mbAskOnlyForFamilyPart)
		s += L"חלק המשפחה, ";

	if (mSeveranceDue > 0)
	{
		s += L"פיצויים ";
		s += ToString(mSeveranceDue);
		s += L" + פנסיה ";
		s += ToString(mPensionDue);
	}
	else
	{
		s += L"פנסיה בלבד";
	}

	return s;
}
void CPension::WriteToLetter(class CHtmlWriter& html)
{
	if (mReport.IsEmpty())
		return;

	html.StartParagraph();
	html.WriteLineEH(L"Computation of due pension by month:", 
		L"חישוב זכאות לפנסיה לפי חודשים:");

	html.WriteEH(L"First day for pension: ", L"חישוב הפנסיה החל מתאריך: ");
	html.WriteLineEH(mStartDateForPension.ToString(), mStartDateForPension.ToHebrewString());

	mReport.WriteToLetter(html);
	html.EndParagraph();
}