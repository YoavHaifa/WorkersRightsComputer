#include "StdAfx.h"
#include "Notice.h"
#include "WorkPeriod.h"
#include "WorkYears.h"
#include "FamilyPart.h"


CNotice::CNotice(void)
	: CRight(L"Notice", L"הודעה מוקדמת")
{
	miPrintOrder = 5;
	mbDemandFullMonthAnyway = false;
}
CNotice::~CNotice(void)
{
}
bool CNotice::Compute(void) 
{
	mDuePay = 0;
	mbValid = false;

	if (gWorkPeriod.mbSkipNotice)
	{
		LogLine(L"Do not demand notice!");
		msDue += L"Notice was not demanded.";
		return false;
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
	bool bHalfDayDue = false;

	if (gWorkYears.mnFullWorkYears >= 1 || mbDemandFullMonthAnyway)
	{
		mbDemandFullMonth = true;
		if (gWorkYears.mnFullWorkYears >= 1)
			LogLine(L"At least one full year worked");
		else
			LogLine(L"Demand full month for less than one full year worked");

		mLastDayOfNotice.AddMonth();
		LogLine(L"Last Day of Notice", mLastDayOfNotice);
		if (mLastDayOfNotice <= gWorkPeriod.mLast)
		{
			LogLine(L"Worked full month of notice period.");
			msDue += L"Worked full month of notice period.";
			return false;
		}
	}
	else
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
			bHalfDayDue = true;
			LogLine(L"Half day due, n days", dueDays);
		}
		mLastDayOfNotice.AddDays(dueDays);
		msDue += ToString(mDueNoticeDays);
		msDue += L" days ";
	}

	LogLine(L"n days in month for salary", gWorkPeriod.mnDaysInMonthForDailySalary);
	LogLine(L"");

	// Count paid work days from notice until last
	mDueWorkDayToPay = 0;
	mnDaysPaidAfterNotice = 0;
	CMyTime checkDate (gWorkPeriod.mNotice.NextDay());
	LogLine(L"Check From", checkDate);
	while (checkDate <= gWorkPeriod.mLast)
	{
		if (gWorkPeriod.maWorkingDays[checkDate.mDayOfWeek - 1] > 0)
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

	if (mbDemandFullMonth && (mnDaysPaidAfterNotice == 0))
	{
		double payPerMonth = gMinWage.PayPerMonthAtWorkEnd();
		msDebug += L"Pay per month ";
		msDebug += ToString(payPerMonth);
		/*
		if (mnDaysPaidAfterNotice > 0)
		{
			double fraction = (gWorkPeriod.mnDaysInMonthForDailySalary - mnDaysPaidAfterNotice) / gWorkPeriod.mnDaysInMonthForDailySalary;
			if (fraction > 1)
				fraction = 1;
			if (fraction < 0)
				fraction = 0;
			msDue += L"Month * Fraction ";
			msDue += ToString(fraction);
			mDuePay = payPerMonth * fraction;
		}
		else */
		msDue += L"Full Month";
		mDuePay = payPerMonth;
	}
	else
	{
		int nDaysChecked = 0;
		while (checkDate <= mLastDayOfNotice)
		{
			int dayOf = checkDate.mDayOfWeek - 1;
			if (gWorkPeriod.maWorkingDays[dayOf] > 0)
			{
				mDueWorkDayToPay++;

				if (checkDate.mTime == mLastDayOfNotice.mTime)
				{
					if (bHalfDayDue)
					{
						mDueWorkDayToPay -= 0.5;
						LogLine(L"Add Last Half Day", checkDate.mTime, L"=", mDueWorkDayToPay);
					}
					else
						LogLine(L"Add Last Day", checkDate.mTime, L"=", mDueWorkDayToPay);
				}
				else
					LogLine(L"Add Day", checkDate.mTime, L"=", mDueWorkDayToPay);
			}
			checkDate.AddDay();
			nDaysChecked++;

			if (mDueWorkDayToPay + mnDaysPaidAfterNotice >= gWorkPeriod.mnDaysInMonthForDailySalary)
			{
				LogLine(L"No more days in salary's month", gWorkPeriod.mnDaysInMonthForDailySalary);
				break;
			}
		}
		LogLine(L"");

		if (mDueWorkDayToPay > gWorkPeriod.mnDaysInMonthForDailySalary)
		{
			mDueWorkDayToPay = gWorkPeriod.mnDaysInMonthForDailySalary;
			msDebug += L" (Full Month)";
			LogLine(L"n due days < n days in salary month", gWorkPeriod.mnDaysInMonthForDailySalary);
		}
		int nWeeksChecked = (nDaysChecked + gWorkPeriod.N_WEEK_DAYS - 1) / gWorkPeriod.N_WEEK_DAYS;
		if (mDueWorkDayToPay > (gWorkPeriod.N_WEEK_DAYS - 1) * nWeeksChecked)
		{
			mDueWorkDayToPay = (gWorkPeriod.N_WEEK_DAYS - 1) * nWeeksChecked;
			msDebug += L" (No more than 6 days per week)";
			LogLine(L"n weeks checked", nWeeksChecked);
			LogLine(L"n due days < (gWorkPeriod.N_WEEK_DAYS - 1) * nWeeksChecked", mDueWorkDayToPay);
		}

		msDue += L" - extra "; // mDuePay.ToString("F2");
		msDue += ToString(mDueWorkDayToPay);
		msDue += L" days ";
		mPayPerDay = gMinWage.PayPerDayAtWorkEnd();
		LogLine(L"Pay per day at work end", mPayPerDay);

		msDue += L" * ";
		msDue += ToString(mPayPerDay);
		mDuePay = mDueWorkDayToPay * mPayPerDay;
	}
	msDue += L" ==> ";
	msDue += ToString(mDuePay);
	mbValid = true;

	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		mDuePay = mDuePay * gFamilyPart.mRatio;
		msDue += L" =FamilyPart=> ";
		msDue += ToString(mDuePay);
	}

	return true;
}
CString CNotice::GetDecriptionForLetter(void)
{
	CString s;
	if (mbDemandFullMonth && (mnDaysPaidAfterNotice == 0))
	{
		s = L"Full month";
	}
	else
	{
		s += ToString(mDueWorkDayToPay);
		s += L" days * ";
		s += ToString(mPayPerDay);
	}
	return s;
}
CString CNotice::GetDecriptionForLetterHebrew(void)
{
	CString s;
	if (mbDemandFullMonth && (mnDaysPaidAfterNotice == 0))
	{
		s = L"חודש מלא";
	}
	else
	{
		s += ToString(mDueWorkDayToPay);
		s += L" ימים * ";
		s += ToString(mPayPerDay);
	}
	return s;
}
