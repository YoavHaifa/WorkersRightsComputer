#include "stdafx.h"
#include "MonthInfo.h"
#include "WorkPeriod.h"
#include "Utils.h"


CMonthInfo::CMonthInfo()
	: mi(-1)
	, mSeniorityMonths(-1)
	, mnAllWorkingDays(0)
	, mnDaysUnpaidVacation(0)
	, mnDaysWorked(0)
	, mFraction(1)
{
}
CMonthInfo::~CMonthInfo()
{
}
void CMonthInfo::InitFirst()
{
	Set(gWorkPeriod.mFirst.mTime);
	SetDaysPerWeek();

	mi = 0;
	mSeniorityMonths = 0;
	mnAllWorkingDays = gWorkPeriod.CountWorkingDaysPerMonth(mYear,mMonth);
	mnDaysWorked = gWorkPeriod.CountDaysWorkedPerMonth(mYear,mMonth);
	mFraction = (double)mnDaysWorked / mnAllWorkingDays;
	mnDaysUnpaidVacation = 0;
	mbLast = MonthContains(gWorkPeriod.mLast);
}
void CMonthInfo::InitNext(CMonthInfo &prev)
{
	Set(prev.mYear, prev.mMonth, 1);
	AddMonth();
	SetDaysPerWeek();

	mi = prev.mi + 1;
	if (mi == 1 && prev.IsPartial())
		mSeniorityMonths = 0;
	else
		mSeniorityMonths = prev.mSeniorityMonths + 1;

	if (MonthContains(gWorkPeriod.mLast))
	{
		mnAllWorkingDays = gWorkPeriod.CountWorkingDaysPerMonth(mYear,mMonth);
		mnDaysWorked = gWorkPeriod.CountDaysWorkedPerMonth(mYear,mMonth);
		mFraction = (double)mnDaysWorked / mnAllWorkingDays;
		mbLast = true;
	}
	else
	{
		mnAllWorkingDays = 0;
		mnDaysWorked = 0;
		mFraction = 1;
		mbLast = false;
	}
	mnDaysUnpaidVacation = 0;
}
void CMonthInfo::SetDaysPerWeek(void)
{
	if (mYear < 2018)
		mHoursPerWeek = 43;
	else if (mYear == 2018 && mMonth < 4)
		mHoursPerWeek = 43;
	else
		mHoursPerWeek = 42;
}
bool CMonthInfo::IsPartial(void)
{
	return (mFraction < 1);
}
void CMonthInfo::SetUnpaid(int nAnpaidDaysToAdd)
{
	if (nAnpaidDaysToAdd < 1)
		return;

	if (mnAllWorkingDays == 0)
	{
		mnAllWorkingDays = gWorkPeriod.CountWorkingDaysPerMonth(mYear, mMonth);
		mnDaysWorked = gWorkPeriod.CountDaysWorkedPerMonth(mYear, mMonth);
		mnDaysUnpaidVacation = nAnpaidDaysToAdd;
	}
	else
		mnDaysUnpaidVacation += nAnpaidDaysToAdd;
	mnDaysWorked -= nAnpaidDaysToAdd;
		
	mFraction = (double)mnDaysWorked / mnAllWorkingDays;

	if (mFraction < 0)
		CUtils::MessBox(L"<CMonthInfo::SetUnpaid> fraction < 0", L"SW Error");
	if (mi > 0 && !mbLast)
	{
		if (mnDaysWorked + mnDaysUnpaidVacation != mnAllWorkingDays)
			CUtils::MessBox(L"<CMonthInfo::SetUnpaid> Paid and unpaid days don't sum correctly", L"SW Error");
	}
}
