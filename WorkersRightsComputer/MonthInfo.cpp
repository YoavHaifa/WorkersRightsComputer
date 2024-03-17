#include "stdafx.h"
#include "MonthInfo.h"
#include "WorkPeriod.h"
#include "Wage.h"
#include "MinWage.h"
#include "Utils.h"
#include "MonthlyFamilyPart.h"


CMonthInfo::CMonthInfo()
	: mi(-1)
	, mSeniorityMonths(-1)
	, mnDaysUnpaidVacation(0)
	, mnDaysNoPension(0)
	, mnDaysWorked(0)
	, mnDaysWorkedForPension(0)
	, mMonthFraction(1)
	, mMonthFractionForPension(1)
	, mbInitializedBeforeVacation(false)
{
}
CMonthInfo::~CMonthInfo()
{
}
void CMonthInfo::ComputeFractionBeforeVacations()
{
	int nWorkingDaysPerMonth = gWorkPeriod.CountWorkingDaysPerMonth(mFirstDay);
	mnDaysWorked = gWorkPeriod.CountDaysWorkedPerMonth(mFirstDay);
	mnDaysWorkedForPension = mnDaysWorked;
	if (nWorkingDaysPerMonth > 0)
		mMonthFraction = (double)mnDaysWorked / nWorkingDaysPerMonth;
	else
		mMonthFraction = 0;
	mMonthFractionForPension = mMonthFraction;
	mbInitializedBeforeVacation = true;
}
void CMonthInfo::InitFirst()
{
	CMyTime dayAfter(gWorkPeriod.mFirst.NextMonth().FirstDayOfMonth());
	CWorkSpan::InitWorkSpan(gWorkPeriod.mFirst, dayAfter);
	if (!Debug(L"CMonthInfo::InitFirst"))
	{
		static int unErr = 0;
		unErr++;
	}
	mHoursPerWeek = gWorkPeriod.GetWorkingHoursInFullWeek(mFirstDay);

	mi = 0;
	mSeniorityMonths = 0;
	ComputeFractionBeforeVacations();
	mnDaysUnpaidVacation = 0;
	mnDaysNoPension = 0;
	mbLast = Contains(gWorkPeriod.mLast);
}
void CMonthInfo::InitNext(CMonthInfo &prev)
{
	static bool bDebug = false;
	CMyTime first(prev.mDayAfter);
	CMyTime dayAfter(first.NextMonth());
	if (dayAfter.mYear == 2017)
	{
		bDebug++;
	}
	CWorkSpan::InitWorkSpan(first, dayAfter);
	if (!Debug(L"CMonthInfo::InitNext"))
	{
		static int unErr = 0;
		unErr++;
	}
	mHoursPerWeek = gWorkPeriod.GetWorkingHoursInFullWeek(mFirstDay);

	mi = prev.mi + 1;
	if (mi == 1 && prev.IsPartial())
		mSeniorityMonths = 0;
	else
		mSeniorityMonths = prev.mSeniorityMonths + 1;

	if (Contains(gWorkPeriod.mLast))
	{
		ComputeFractionBeforeVacations();
		mbLast = true;
	}
	else
	{
		mnDaysWorked = 0;
		mnDaysWorkedForPension = 0;
		mMonthFraction = 1;
		mMonthFractionForPension = 1;
		mbLast = false;
		mbInitializedBeforeVacation = false;
	}
	mnDaysUnpaidVacation = 0;
	mnDaysNoPension = 0;
}
bool CMonthInfo::IsPartial(void)
{
	return (mMonthFraction < 1);
}
/*
double CMonthInfo::GetFamilyRatio(CString* posCompanyRatio, double* poCompanyHourlyWage)
{
	// If nothing else is defined
	double familyRatio = 1;
	if (posCompanyRatio)
		*posCompanyRatio = "0";
	if (poCompanyHourlyWage)
		*poCompanyHourlyWage = mHourlyRateByCompany;

	if (mHoursPerWeekPaidByCompany > 0)
	{
		// Defined by hours
		if (posCompanyRatio)
		{
			char zBuf[128];
			sprintf_s(zBuf, sizeof(zBuf), "%5.2f", mHoursPerWeekPaidByCompany);
			*posCompanyRatio = zBuf;
		}
		familyRatio = 1 - mHoursPerWeekPaidByCompany / mHoursPerWeek;
	}
	else if (mRatioPaidByCompany)
	{
		// Defined by ratio
		if (posCompanyRatio)
		{
			char zBuf[128];
			sprintf_s(zBuf, sizeof(zBuf), "%5.2f%%", mRatioPaidByCompany * 100);
			*posCompanyRatio = zBuf;
		}
		familyRatio = 1.0 - mRatioPaidByCompany;
	}

	if (familyRatio < 1.0)
	{
		double monthlyAgreed = gWage.GetMonthlyWageFor(mFirstDay);
		if (monthlyAgreed > 0)
		{
			double fullCompanyPay = gMinWage.GetMonthlyWage(mFirstDay);
			if (mHourlyRateByCompany > 0)
			{
				int nWorkHoursInFullMonth = gWorkPeriod.GetWorkingHoursInFullMonth(mFirstDay);
				fullCompanyPay = mHourlyRateByCompany * nWorkHoursInFullMonth;
			}
			if (monthlyAgreed > fullCompanyPay)
			{
				double ratioParallelEmployment = fullCompanyPay / monthlyAgreed;
				double ratioFamilyAlone = 1 - ratioParallelEmployment;
				double familyRatioInParellel = familyRatio;
				familyRatio = ratioParallelEmployment * familyRatio + ratioFamilyAlone;
				if (posCompanyRatio)
				{
					char zBuf[128];
					sprintf_s(zBuf, sizeof(zBuf), ", Agreed %5.2f > %5.2f: Initial Family %5.2f%% of %5.2f%% parallel + %5.2f%%", 
						monthlyAgreed, fullCompanyPay,
						familyRatioInParellel * 100, ratioParallelEmployment * 100, ratioFamilyAlone * 100);
					*posCompanyRatio += zBuf;
				}
			}
		}
	}
	return familyRatio;
}*/
void CMonthInfo::Log(FILE* pf)
{
	fwprintf(pf, L"%3d: %d/%02d - ", mi, mFirstDay.mYear, mFirstDay.mMonth);

	if (mMonthFraction < 1)
	{
		if (mnDaysWorked < mnWorkDays)
			fwprintf(pf, L" worked %d/%d", mnDaysWorked, mnWorkDays);

		fwprintf(pf, L" fraction %.3f", mMonthFraction);
	}
	if (mMonthFractionForPension < 1)
		fwprintf(pf, L" pension fraction %.3f", mMonthFractionForPension);

	if (mbLast)
		fwprintf(pf, L" LAST\n");

	fwprintf(pf, L"\n");
}
void CMonthInfo::LogFraction(FILE* pf)
{
	fwprintf(pf, L"%3d: %d/%02d - %0.2f (pension %0.2f)\n", mi,
		mFirstDay.mYear, mFirstDay.mMonth, mMonthFraction, mMonthFractionForPension);
}
void CMonthInfo::SetUnpaid(int nUnpaidDaysToAdd)
{
	if (nUnpaidDaysToAdd < 1)
		return;

	if (!mbInitializedBeforeVacation)
	{
		ComputeFractionBeforeVacations();
		mnDaysUnpaidVacation = nUnpaidDaysToAdd;
	}
	else
		mnDaysUnpaidVacation += nUnpaidDaysToAdd;
	mnDaysWorked -= nUnpaidDaysToAdd;

	if (mnWorkDays > 0)
		mMonthFraction = (double)mnDaysWorked / mnWorkDays;
	else
		mMonthFraction = 0;

	if (mMonthFraction < 0)
		CUtils::MessBox(L"<CMonthInfo::SetUnpaid> fraction < 0", L"SW Error");
	if (mi > 0 && !mbLast)
	{
		if (mnDaysWorked + mnDaysUnpaidVacation != mnWorkDays)
			CUtils::MessBox(L"<CMonthInfo::SetUnpaid> Paid and unpaid days don't sum correctly", L"SW Error");
	}
}
void CMonthInfo::SetNoPension(int nDaysToAdd)
{
	if (nDaysToAdd < 1)
		return;

	if (!mbInitializedBeforeVacation)
	{
		ComputeFractionBeforeVacations();
		mnDaysNoPension = nDaysToAdd;
	}
	else
		mnDaysNoPension += nDaysToAdd;
	mnDaysWorkedForPension -= nDaysToAdd;

	if (mnWorkDays > 0)
		mMonthFractionForPension = (double)mnDaysWorkedForPension / mnWorkDays;
	else
		mMonthFractionForPension = 0;

	if (mMonthFractionForPension < 0)
		CUtils::MessBox(L"<CMonthInfo::SetNoPension> FractionForPension < 0", L"SW Error");
	if (mi > 0 && !mbLast)
	{
		if (mnDaysWorkedForPension + mnDaysNoPension != mnWorkDays)
			CUtils::MessBox(L"<CMonthInfo::SetNoPension> Pension and no pension days don't sum correctly", L"SW Error");
	}
}
int CMonthInfo::GetNRelevantDaysIn(CDaysSpan& span, bool bPension)
{
	if (!mbInitializedBeforeVacation)
		ComputeFractionBeforeVacations();

	// Assume that if days were subtracted before - they must be from a previous span
	if (bPension)
		return min (mnDaysWorkedForPension, span.mnWorkDays);
	return min (mnDaysWorked, span.mnWorkDays);
}
bool CMonthInfo::Debug(const wchar_t* zAt)
{
	if (mnDays > 31)
	{
		wchar_t zBuf[128];
		swprintf_s(zBuf, 128, L"<CMonthInfo::Debug> at %s: mnDays %d > 31", zAt, mnDays);
		CUtils::MessBox(zBuf, L"SW Error");
		return false;
	}

	int nOff = mnDays / 7;
	if (mnWorkDays > mnDays - nOff)
	{
		wchar_t zBuf[128];
		swprintf_s(zBuf, 128, L"<CMonthInfo::Debug> at %s: mnWorkDays %d > (%d mnDays - %d)",
			zAt, mnWorkDays, mnDays, nOff);
		CUtils::MessBox(zBuf, L"SW Error");
		return false;
	}
	if (mFirstDay.mYear != mLastDay.mYear || mFirstDay.mMonth != mLastDay.mMonth)
	{
		wchar_t zBuf[128];
		swprintf_s(zBuf, 128, L"<CMonthInfo::Debug> at %s: not a month! %d.%d != %d.%d",
			zAt,mFirstDay.mYear, mFirstDay.mMonth, mLastDay.mYear, mLastDay.mMonth);
		CUtils::MessBox(zBuf, L"SW Error");
		return false;
	}
	return true;
}