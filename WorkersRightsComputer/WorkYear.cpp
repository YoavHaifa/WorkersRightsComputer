#include "StdAfx.h"
#include "WorkYear.h"
#include "WorkPeriod.h"
#include "UsedVacations.h"
#include "VacationUsed.h"

CWorkYear::CWorkYear(void)
	: mSeniority(0)
	, mbLast(false)
	, mnUnpaidVacationDays(0)
{
}
CWorkYear::~CWorkYear(void)
{
}
void CWorkYear::InitFirst(void)
{
	mSeniority = 1;
	mFirstDay = gWorkPeriod.mFirst;
	InitInternals();
}
void CWorkYear::InitNext(CWorkYear& prev)
{
	mSeniority = prev.mSeniority + 1;
	mFirstDay = prev.mDayAfter;
	InitInternals();
}
void CWorkYear::InitInternals(void)
{
	mnUnpaidVacationDays = 0;
	mbLast = false;
	mDayAfter = mFirstDay;
	mDayAfter.AddYears(1);

	// Add unpaid vacations
	gUsedVacations.AddToWorkYear(*this);

	if (mDayAfter > gWorkPeriod.mLast)
		mbLast = true;
}
void CWorkYear::AddUnpaidVacation(class CVacationUsed& vacation)
{
	mDayAfter.AddDays(vacation.mnUnpaidCalendarDays);
	mnUnpaidVacationDays += vacation.mnUnpaidCalendarDays;
}
void CWorkYear::Log(FILE* pfLog)
{
	if (!pfLog)
		return;
	fprintf(pfLog, "%2d: ", mSeniority);
	mFirstDay.Log(pfLog);
	fprintf(pfLog, "- ");
	mDayAfter.Log(pfLog);
	if (mnUnpaidVacationDays > 0)
		fprintf(pfLog, "Unpaid Vacation %d Days", mnUnpaidVacationDays);
	fprintf(pfLog, "\n");
}
