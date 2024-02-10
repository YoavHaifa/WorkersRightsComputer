#include "StdAfx.h"
#include "Notice.h"
#include "WorkPeriod.h"
#include "WorkYears.h"
#include "FamilyPart.h"
#include "WageTable.h"
#include "Config.h"

CNotice::CNotice()
	: CRight(L"Notice", L"הודעה מוקדמת")
	, mFamilyRatio(1)
{
	miPrintOrder = 5;
}
CNotice::~CNotice()
{
}
bool CNotice::ComputeDemandFullMonth()
{
	mbDemandFullMonth = true;
	if (gWorkYears.mnFullWorkYears >= 1)
		LogLine(L"At least one full year worked");
	else
		LogLine(L"Demand full month for less than one full year worked");

	mLastDayOfNotice.AddMonth();
	gWorkPeriod.SetLastDayOfNotice(mLastDayOfNotice);
	LogLine(L"Last Day of Notice", mLastDayOfNotice);
	if (mLastDayOfNotice <= gWorkPeriod.mLast)
	{
		LogLine(L"Worked full month of notice period.");
		msDue += L"Worked full month of notice period.";
		return false;
	}
	return true;
}
bool CNotice::ComputeForLessThanAYear()
{
	mbDemandFullMonth = false;
	if (gWorkYears.mnMonthsInLastYear < 7)
	{
		mDueNoticeDays = gWorkYears.mnMonthsInLastYear;
		LogLine(L"Less than 7 months, demand day per month", mDueNoticeDays);
	}
	else
	{
		int extraMonths = gWorkYears.mnMonthsInLastYear - 6;
		mDueNoticeDays = 6 + extraMonths * 2.5;
		LogLine(L"More than half year, demand days", mDueNoticeDays);
	}
	int dueDays = (int)mDueNoticeDays;
	if (dueDays < mDueNoticeDays)
	{
		dueDays++;
		if (!gConfig.mbBackwardCompatibilityMode || gConfig.miLegacyVersion >= 133)
		{
			mDueNoticeDays = dueDays;
			LogLine(L"Rounded up by half day, n days due", dueDays);
		}
		else
			mbAvoidRoundingLastHalfDay = true;
	}
	mLastDayOfNotice.AddDays(dueDays);
	gWorkPeriod.SetLastDayOfNotice(mLastDayOfNotice);
	LogLine(L"Last Day of Notice", mLastDayOfNotice);

	if (mLastDayOfNotice <= gWorkPeriod.mLast)
	{
		LogLine(L"Worked all of notice period.");
		msDue += L"Worked all of notice period.";
		return false;
	}
	msDue += ToString(mDueNoticeDays);
	msDue += L" days ";
	return true;
}
void CNotice::CheckForPaidDaysAfterNotice()
{
	mnDaysPaidAfterNotice = 0;
	CMyTime checkDate(gWorkPeriod.mNotice.NextDay());
	LogLine(L"Check From", checkDate);
	while (checkDate <= gWorkPeriod.mLast)
	{
		if (checkDate.IsWorkingDay())
		{
			mnDaysPaidAfterNotice++;
			LogLine(L"Worked", checkDate.mTime, L"Paid", mnDaysPaidAfterNotice);
		}
		checkDate.AddDay();
	}

	LogLine(L"n days paid after notice", mnDaysPaidAfterNotice);
	LogLine(L"");
	if (mnDaysPaidAfterNotice > 0)
	{
		msDebug += L" (Days paid after notice ";
		msDebug += ToString(mnDaysPaidAfterNotice);
		msDebug += L") ";
	}
}
void CNotice::CountWorkDaysToPay()
{
	int nDaysChecked = 0;
	CMyTime checkDate(gWorkPeriod.mLast.NextDay());
	while (checkDate <= mLastDayOfNotice)
	{
		if (checkDate.IsWorkingDay())
		{
			mDueWorkDaysToPay++;
			LogLine(L"Add Day", checkDate.mTime, L"=", mDueWorkDaysToPay);
		}
		checkDate.AddDay();
		nDaysChecked++;

		if (mDueWorkDaysToPay + mnDaysPaidAfterNotice >= gWorkPeriod.mnDaysInMonthForDailySalary)
		{
			LogLine(L"No more days in salary's month", gWorkPeriod.mnDaysInMonthForDailySalary);
			break;
		}
	}
	if (mbAvoidRoundingLastHalfDay)
	{
		if (mDueWorkDaysToPay > 0.5)
		{
			mDueWorkDaysToPay -= 0.5;
			LogLine(L"Reduce 0.5 rounded-up day for backward compatibility", mDueWorkDaysToPay);
		}
	}

	LogLine(L"");
	if (mDueWorkDaysToPay > gWorkPeriod.mnDaysInMonthForDailySalary)
	{
		mDueWorkDaysToPay = gWorkPeriod.mnDaysInMonthForDailySalary;
		msDebug += L" (Full Month)";
		LogLine(L"n due days was > n days in salary month", gWorkPeriod.mnDaysInMonthForDailySalary);
	}
	int nWeeksChecked = (nDaysChecked + gWorkPeriod.N_WEEK_DAYS - 1) / gWorkPeriod.N_WEEK_DAYS;
	if (mDueWorkDaysToPay > (gWorkPeriod.N_WEEK_DAYS - 1)* nWeeksChecked)
	{
		mDueWorkDaysToPay = (gWorkPeriod.N_WEEK_DAYS - 1) * nWeeksChecked;
		msDebug += L" (No more than 6 days per week)";
		LogLine(L"n weeks checked", nWeeksChecked);
		LogLine(L"n due days < (gWorkPeriod.N_WEEK_DAYS - 1) * nWeeksChecked", mDueWorkDaysToPay);
	}
}
bool CNotice::Compute() 
{
	mDuePay = 0;
	mbValid = false;
	mbAvoidRoundingLastHalfDay = false;
	gWorkPeriod.SetLastDayOfNotice(gWorkPeriod.mLast); // No Notice yet

	if (gWorkPeriod.mbSkipNotice)
	{
		LogLine(L"Do not demand notice!");
		msDue += L"Notice was not demanded.";
		return true;
	}
	if (!gWorkPeriod.mNotice.mbInitialized)
	{
		LogLine(L"Notice Date not set!");
		msDue += L"Notice Date not set.";
		return false;
	}

	LogLine(L"Last day at work", gWorkPeriod.mLast);
	LogLine(L"Notice", gWorkPeriod.mNotice);
	LogLineSpan(L"Last - Notice ", gWorkPeriod.mNotice, gWorkPeriod.mLast);

	// Compute to how many days the worker is entitled 
	// and what is the last day to which he is entitled to be paid
	mDueNoticeDays = 0;
	mLastDayOfNotice = gWorkPeriod.mNotice;

	if (gWorkYears.mnFullWorkYears >= 1)
	{
		if (!ComputeDemandFullMonth())
			return true;
	}
	else
	{
		if (!ComputeForLessThanAYear())
			return true;
	}

	LogLine(L"n days in month for salary", gWorkPeriod.mnDaysInMonthForDailySalary);
	LogLine(L"");

	// Count paid work days from notice until last
	CheckForPaidDaysAfterNotice();

	mDueWorkDaysToPay = 0;
	if (mbDemandFullMonth && (mnDaysPaidAfterNotice == 0))
	{
		double payPerMonth = gWageTable.PayPerMonthAtWorkEnd();
		msDebug += L"Pay per month ";
		msDebug += ToString(payPerMonth);
		msDue += L"Full Month";
		mDuePay = payPerMonth;
	}
	else
	{
		CountWorkDaysToPay();

		msDue += L" - extra ";
		msDue += ToString(mDueWorkDaysToPay);
		msDue += L" days ";
		mPayPerDay = gWageTable.PayPerDayAtWorkEnd();
		LogLine(L"Pay per day at work end", mPayPerDay);

		msDue += L" * ";
		msDue += ToString(mPayPerDay);
		mDuePay = mDueWorkDaysToPay * mPayPerDay;
	}
	msDue += L" ==> ";
	msDue += ToString(mDuePay);
	mbValid = true;

	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		bool bLast3Month = false;
		if (gConfig.mbBackwardCompatibilityMode && gConfig.miLegacyVersion < 133)
		{
			mFamilyRatio = gWorkPeriod.ComputeFamilyPart();
			LogLine(L"Family part (whole period, backward compatibility mode)", mFamilyRatio);
		}
		else
		{
			bLast3Month = true;
			mFamilyRatio = gWorkPeriod.ComputeFamilyPartLastMonths(umn3MonthsForFamilyPart);
			LogLine(L"Family part - average last 3 month", mFamilyRatio);
		}

		msFamilyRatio = CFamilyPart::Ratio2S(mFamilyRatio);
		mDuePay = mDuePay * mFamilyRatio;
		msDue += L" = Family Part ";
		msDue += msFamilyRatio;
		if (bLast3Month)
			msDue += L" (Last 3 Months)";
		msDue += L" ==> ";
		msDue += ToString(mDuePay);
	}

	return true;
}
CString CNotice::GetDecriptionForLetter()
{
	CString s;

	if (mbDemandFullMonth && (mnDaysPaidAfterNotice == 0))
	{
		s = L"Full month";
	}
	else
	{
		s += ToString(mDueWorkDaysToPay);
		s += L" days * ";
		s += ToString(mPayPerDay);
	}

	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		s += ", Family Part (last 3 month) ";
		s += msFamilyRatio;
	}
	return s;
}
CString CNotice::GetDecriptionForLetterHebrew()
{
	CString s;

	if (mbDemandFullMonth && (mnDaysPaidAfterNotice == 0))
	{
		s = L"חודש מלא";
	}
	else
	{
		s += ToString(mDueWorkDaysToPay);
		s += L" ימים * ";
		s += ToString(mPayPerDay);
	}
	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		s += L",חלק המשפחה (3 חודשים אחרונים)  ";
		s += msFamilyRatio;
	}
	return s;
}
