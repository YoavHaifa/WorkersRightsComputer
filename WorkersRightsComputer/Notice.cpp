#include "StdAfx.h"
#include "Notice.h"
#include "WorkPeriod.h"
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

	if (!gWorkPeriod.mNotice.mbInitialized)
	{
		LogLine(L"Notice Date not set!");
		msDue += L"Notice Date not set.";
		return false;
	}

	LogLine(L"Last day at work", gWorkPeriod.mLast.mTime);
	LogLine(L"Notice", gWorkPeriod.mNotice.mTime);
	CTimeSpan spanAfterNotice = gWorkPeriod.mLast.mTime - gWorkPeriod.mNotice.mTime;
	LogLine(L"Last - Notice ", spanAfterNotice);

	// Compute to how many days the worker is entitled 
	// and what is the last day to which he is entitled to be paid
	mDueNoticeDays = 0;
	mLastDayOfNotice = gWorkPeriod.mNotice;
	bool bHalfDayDue = false;

	if (gWorkPeriod.mnFullWorkYears >= 1 || mbDemandFullMonthAnyway)
	{
		mbDemandFullMonth = true;
		mLastDayOfNotice = gWorkPeriod.mNotice;
		mLastDayOfNotice.AddMonth();
		msDue += L"Month ";
		if (gWorkPeriod.mnFullWorkYears >= 1)
			LogLine(L"At least one full year worked");
		else
			LogLine(L"Demand full month for less than one full year worked");
	}
	else
	{
		mbDemandFullMonth = false;
		if (gWorkPeriod.mnMonths < 7)
		{
			mDueNoticeDays = gWorkPeriod.mnMonths;
			LogLine(L"Less than 7 months, demand day per month", mDueNoticeDays);
		}
		else
		{
			int extraMonths = gWorkPeriod.mnMonths - 6;
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

	// Count Days Until last
	mDueWorkDayToPay = 0;
	mnDaysPaidAfterNotice = 0;
	CMyTime checkDate (gWorkPeriod.mNotice.NextDay());
	LogLine(L"Check", checkDate.mTime);
	while (checkDate.mTime <= gWorkPeriod.mLast.mTime)
	{
		CTimeSpan span = gWorkPeriod.mLast.mTime - checkDate.mTime;
		LogLine(L"In loop", span);
		int dayOf = checkDate.mDayOfWeek - 1;
		if (gWorkPeriod.maWorkingDays[dayOf] > 0)
		{
			mnDaysPaidAfterNotice++;
			LogLine(L"Worked", checkDate.mTime, L"Paid", mnDaysPaidAfterNotice);
		}
		checkDate.AddDay();
		LogLine(L"Check", checkDate.mTime);
	}
	LogLine(L"Check Time", (__int64)checkDate.mTime.GetTime());
	LogLine(L"Last Time", (__int64)gWorkPeriod.mLast.mTime.GetTime());
	CTimeSpan spanAfterLast = checkDate.mTime - gWorkPeriod.mLast.mTime;
	LogLine(L"Out of loop", spanAfterLast);

	LogLine(L"n days paid after notice", mnDaysPaidAfterNotice);
	LogLine(L"");
	if (mnDaysPaidAfterNotice > 0)
	{
		msDebug += L" (Days paid after notice ";
		msDebug += ToString(mnDaysPaidAfterNotice);
		msDebug += L") ";
	}

	int nDaysChecked = 0;
	while (checkDate.mTime <= mLastDayOfNotice.mTime)
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
	msDue += L" ==> ";
	mDuePay = mDueWorkDayToPay * mPayPerDay;
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
	s += ToString(mDueWorkDayToPay);
	s += L" days * ";
	s += ToString(mPayPerDay);

	return s;
}
CString CNotice::GetDecriptionForLetterHebrew(void)
{
	CString s;
	s += ToString(mDueWorkDayToPay);
	s += L" ימים * ";
	s += ToString(mPayPerDay);

	return s;
}
