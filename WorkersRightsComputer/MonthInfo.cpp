#include "stdafx.h"
#include "MonthInfo.h"
#include "WorkPeriod.h"
#include "Utils.h"


CMonthInfo::CMonthInfo()
	: mi(-1)
	, mSeniorityMonths(-1)
	, mnAllDays(0)
	, mnAllWorkingDays(0)
	, mnDaysUnpaidVacation(0)
	, mnDaysNoPension(0)
	, mnDaysWorked(0)
	, mnDaysWorkedForPension(0)
	, mMonthFraction(1)
	, mMonthFractionForPension(1)
{
}
CMonthInfo::~CMonthInfo()
{
}
void CMonthInfo::ComputeFractionBeforeVacations()
{
	mnAllWorkingDays = gWorkPeriod.CountWorkingDaysPerMonth(mYear, mMonth);
	mnDaysWorked = gWorkPeriod.CountDaysWorkedPerMonth(mYear, mMonth);
	mnDaysWorkedForPension = mnDaysWorked;
	mMonthFraction = (double)mnDaysWorked / mnAllWorkingDays;
	mMonthFractionForPension = mMonthFraction;
}
void CMonthInfo::InitFirst()
{
	Set(gWorkPeriod.mFirst.mTime);
	mHoursPerWeek = gWorkPeriod.GetWorkingHoursInFullWeek(mYear, mMonth);

	mi = 0;
	mSeniorityMonths = 0;
	mnAllDays = gWorkPeriod.CountAllDaysPerMonth(mYear,mMonth);
	ComputeFractionBeforeVacations();
	mnDaysUnpaidVacation = 0;
	mnDaysNoPension = 0;
	mbLast = MonthContains(gWorkPeriod.mLast);
}
void CMonthInfo::InitNext(CMonthInfo &prev)
{
	Set(prev.mYear, prev.mMonth, 1);
	AddMonth();
	mHoursPerWeek = gWorkPeriod.GetWorkingHoursInFullWeek(mYear, mMonth);

	mi = prev.mi + 1;
	if (mi == 1 && prev.IsPartial())
		mSeniorityMonths = 0;
	else
		mSeniorityMonths = prev.mSeniorityMonths + 1;

	mnAllDays = gWorkPeriod.CountAllDaysPerMonth(mYear, mMonth);
	if (MonthContains(gWorkPeriod.mLast))
	{
		ComputeFractionBeforeVacations();
		mbLast = true;
	}
	else
	{
		mnAllWorkingDays = 0;
		mnDaysWorked = 0;
		mnDaysWorkedForPension = 0;
		mMonthFraction = 1;
		mMonthFractionForPension = 1;
		mbLast = false;
	}
	mnDaysUnpaidVacation = 0;
	mnDaysNoPension = 0;
}
bool CMonthInfo::IsPartial(void)
{
	return (mMonthFraction < 1);
}
double CMonthInfo::GetCompanyRatio(double* poCompanyHours, double* poCompanyRatio)
{
	if (mHoursPerWeekPaidByCompany > 0)
	{
		// Defined by hours
		if (poCompanyHours)
			*poCompanyHours = mHoursPerWeekPaidByCompany;
		return mHoursPerWeekPaidByCompany / mHoursPerWeek;
	}

	// Defined by ratio
	if (poCompanyRatio)
		*poCompanyRatio = mRatioPaidByCompany;
	return mRatioPaidByCompany;
}
double CMonthInfo::GetFamilyPart(double* poCompanyHours, double* poCompanyRatio)
{
	return 1 - GetCompanyRatio(poCompanyHours, poCompanyRatio);
}
void CMonthInfo::LogFraction(FILE* pf)
{
	fwprintf(pf, L"%3d: %d/%02d - %0.2f (pension %0.2f)\n", mi,
		mYear, mMonth, mMonthFraction, mMonthFractionForPension);
}
void CMonthInfo::SetUnpaid(int nUnpaidDaysToAdd)
{
	if (nUnpaidDaysToAdd < 1)
		return;

	if (mnAllWorkingDays == 0)
	{
		ComputeFractionBeforeVacations();
		mnDaysUnpaidVacation = nUnpaidDaysToAdd;
	}
	else
		mnDaysUnpaidVacation += nUnpaidDaysToAdd;
	mnDaysWorked -= nUnpaidDaysToAdd;

	mMonthFraction = (double)mnDaysWorked / mnAllWorkingDays;

	if (mMonthFraction < 0)
		CUtils::MessBox(L"<CMonthInfo::SetUnpaid> fraction < 0", L"SW Error");
	if (mi > 0 && !mbLast)
	{
		if (mnDaysWorked + mnDaysUnpaidVacation != mnAllWorkingDays)
			CUtils::MessBox(L"<CMonthInfo::SetUnpaid> Paid and unpaid days don't sum correctly", L"SW Error");
	}
}
void CMonthInfo::SetNoPension(int nDaysToAdd)
{
	if (nDaysToAdd < 1)
		return;

	if (mnAllWorkingDays == 0)
	{
		ComputeFractionBeforeVacations();
		mnDaysNoPension = nDaysToAdd;
	}
	else
		mnDaysNoPension += nDaysToAdd;
	mnDaysWorkedForPension -= nDaysToAdd;

	mMonthFractionForPension = (double)mnDaysWorkedForPension / mnAllWorkingDays;

	if (mMonthFractionForPension < 0)
		CUtils::MessBox(L"<CMonthInfo::SetNoPension> FractionForPension < 0", L"SW Error");
	if (mi > 0 && !mbLast)
	{
		if (mnDaysWorkedForPension + mnDaysNoPension != mnAllWorkingDays)
			CUtils::MessBox(L"<CMonthInfo::SetNoPension> Pension and no pension days don't sum correctly", L"SW Error");
	}
}
int CMonthInfo::GetNRelevantDays(bool bPension)
{
	if (mnAllWorkingDays == 0)
		ComputeFractionBeforeVacations();

	if (bPension)
		return mnDaysWorkedForPension;
	return mnDaysWorked;
}