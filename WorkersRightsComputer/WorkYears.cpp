#include "stdafx.h"
#include "WorkYears.h"
#include "WorkYear.h"
#include "WorkPeriod.h"
#include "Utils.h"
#include "Right.h"
#include "UsedVacations.h"

CWorkYears gWorkYears;

CWorkYears::CWorkYears(void)
	: mn(0)
	, mnDaysForSeveranceAddedForUnpaidVacations(0)
	, mbAllowPartYearSeverance(false)
{
}
void CWorkYears::Clear(void)
{
	mn = 0;
}
void CWorkYears::Compute(void)
{
	mn = 0;
	mnFullWorkYears = 0;
	mnPrevYears = 0;
	mYearsForSeverance = 0;
	mnDaysForSeveranceAddedForUnpaidVacations = 0;
	mbAllowPartYearSeverance = false;
	if (!gWorkPeriod.IsValid())
		return;

	maYears[0].InitFirst();
	mn++;
	while (!maYears[mn - 1].mbLast)
	{
		maYears[mn].InitNext(maYears[mn - 1]);
		mn++;
	}

	if (maYears[mn - 1].mFraction == 1)
	{
		mnFullWorkYears = mn;
		mYearsForSeverance = mn;
	}
	else
	{
		mnFullWorkYears = mn - 1;
		mYearsForSeverance = mnFullWorkYears + maYears[mn - 1].mFraction;
	}
	mnPrevYears = mn - 1;

	if (gUsedVacations.mbAdd14DaysUnpaidVacation4Severance)
	{
		for (int i = 0; i < mn; i++)
		{
			mnDaysForSeveranceAddedForUnpaidVacations += maYears[i].GetUnpaidVacationCalendarDaysForSeverance();
		}
		mYearsForSeverance += (double)mnDaysForSeveranceAddedForUnpaidVacations / 365.0;
	}

	double partYearThreshold = (double)N_MIN_MONTHS_FOR_SPECIAL_CASE / 12 - 0.002;
	mbAllowPartYearSeverance = mYearsForSeverance >= partYearThreshold;

	mnMonthsInLastYear = maYears[mn - 1].GetNFullMonths(&mnDaysInLastMonthOfLastYear, &mDaysInLastYearAsFraction);
	ComputeWorkDays(); // WARNING: This procedure dont take into account vacations (as yet)!
}
CMyTime& CWorkYears::GetLastYearStart(void)
{
	return maYears[mn - 1].mFirstDay;
}
CMyTime& CWorkYears::GetPrevYearEnd(int iPrev)
{
	int iYear = mn - 1 - iPrev;
	if (iYear < 0)
	{
		CUtils::MessBox(L"<CWorkYears::GetPrevYearEnd> go back too much", L"SW Error");
		iYear = 0;
	}

	return maYears[iYear].mLastDay;
}
double CWorkYears::GetLastYearAsFraction(void)
{
	return maYears[mn - 1].mFraction;
}
CString CWorkYears::PrepareSpanString(void)
{
	CString sSpanString(L"");
	if (mnFullWorkYears > 0)
	{
		sSpanString = CRight::ToString(mnFullWorkYears);
		sSpanString += " years ";
	}
	if (mn == mnFullWorkYears)
		return sSpanString;

	sSpanString += CRight::ToString(mnMonthsInLastYear);
	sSpanString += " months ";
	sSpanString += CRight::ToString(mnDaysInLastMonthOfLastYear);
	sSpanString += " days";

	if (mnFullWorkYears < 1)
	{
		sSpanString += " (";
		sSpanString += CRight::ToString(mnWorkingDays);
		sSpanString += " work-days)";
	}
	return sSpanString;
}

void CWorkYears::Log()
{
	FILE* pfLog = CUtils::OpenLogFile(L"WorkYears");
	if (!pfLog)
		return;
	if (mn < 1)
	{
		fprintf(pfLog, "No work years defined\n");
	}
	else
	{
		fprintf(pfLog, "%d work years defined\n\n", mn);
		for (int i = 0; i < mn; i++)
		{
			maYears[i].Log(pfLog);
		}
		fprintf(pfLog, "mnMonthsInLastYear          %d\n", mnMonthsInLastYear);
		fprintf(pfLog, "mnDaysInLastMonthOfLastYear %d\n", mnDaysInLastMonthOfLastYear);
		fprintf(pfLog, "mDaysInLastMonthAsFraction  %.2f\n", mDaysInLastYearAsFraction);
		fprintf(pfLog, "mnFullWorkYears             %d\n", mnFullWorkYears);
		fprintf(pfLog, "mnWorkingDays               %d\n", mnWorkingDays);
		fprintf(pfLog, "mYearsForSeverance          %.3f\n", mYearsForSeverance);
	}
	fclose(pfLog);
}
void CWorkYears::ComputeWorkDays(void) // WARNING: This procedure dont take into account vacations (as yet)!
{
	int days = gWorkPeriod.mFirst.GetNDaysUntil(gWorkPeriod.mLast);

	mnWorkingDays = (int)(days * gWorkPeriod.mnWorkDaysPerWeek / 7);
}
bool CWorkYears::WorkedAtLeastNMonths(int nMonths)
{
	int nFullYears = 0;
	while (nMonths >= 12)
	{
		nFullYears++;
		nMonths -= 12;
	}
	if (mnFullWorkYears > nFullYears)
		return true;
	if (mnFullWorkYears < nFullYears)
		return false;

	if (mnMonthsInLastYear >= nMonths)
		return true;
	return false;
}
bool CWorkYears::LastYearDoContains(class CHoliday& holiday)
{
	return maYears[mn-1].Contains(holiday);
}
CWorkYear* CWorkYears::GetByReverseIndex(int iFromLast)
{
	if (iFromLast > mn - 1)
		return NULL;
	return &maYears[mn - 1 - iFromLast];
}