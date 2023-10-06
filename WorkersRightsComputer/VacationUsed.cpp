#include "stdafx.h"
#include "VacationUsed.h"
#include "Right.h"
#include "WorkPeriod.h"
#include "VacationTable.h"
#include "MonthInfo.h"
#include "WorkYear.h"
#include "XMLDump.h"


CVacationUsed::CVacationUsed(CMyTime &firstDay, CMyTime &lastDay)
	: CDaysSpan(firstDay, lastDay)
	, mnPaidDays(0)
	, mnUnPaidWorkDays(0)
	, mnUnpaidUsedForSeverance(0)
	, mbIsMaternityLeave(false)
{
}
CVacationUsed::~CVacationUsed()
{
}
void CVacationUsed::SaveToXml(class CXMLDump& xmlDump)
{
	xmlDump.Write(L"FirstDay", mFirstDay);
	xmlDump.Write(L"LastDay", mLastDay);
}
CString CVacationUsed::GetText()
{
	CString s(mFirstDay.ToString());
	s += " - ";
	s += mLastDay.ToString();
	s += ",  ";
	if (!mbIsMaternityLeave)
	{
		s += CRight::ToString(mnWorkDays);
		s += " / ";
		s += CRight::ToString(mnDays);
		s += " Working Days, Unpaid ";
		s += CRight::ToString(mnUnPaidWorkDays);
	}
	return s;
}
void CVacationUsed::ShortLog(FILE *pf, bool bNewLine)
{
	if (!pf)
		return;
	fprintf(pf, "%d/%d/%d - %d/%d/%d",
		mFirstDay.mDay, mFirstDay.mMonth, mFirstDay.mYear, mLastDay.mDay, mLastDay.mMonth, mLastDay.mYear);
	if (bNewLine)
		fprintf(pf, "\n");
}
void CVacationUsed::LongLog(FILE *pf)
{
	if (!pf)
		return;

	CDaysSpan::Log(pf);
	//fprintf(pf, "%d/%d/%d - %d/%d/%d\n",
	//	mFirstDay.mDay, mFirstDay.mMonth, mFirstDay.mYear, mLastDay.mDay, mLastDay.mMonth, mLastDay.mYear);
	fprintf(pf, "Paid %2d Unpaid %2d\n", mnPaidDays, mnUnPaidWorkDays);
	if (mUnpaidSpan.mFirstDay.mbInitialized)
		mUnpaidSpan.Log(pf, "Unpaid");
	/*
	{
		fprintf(pf, "Unpaid %d/%d/%d - %d/%d/%d, Unpaid calendar days %d\n",
			mUnpaidSpan.mFirstDay.mDay, mUnpaidSpan.mFirstDay.mMonth, mUnpaidSpan.mFirstDay.mYear, 
			mLastDay.mDay, mLastDay.mMonth, mLastDay.mYear, mUnpaidSpan.mnDays);
	}*/
	if (!mbIsMaternityLeave)
		fprintf(pf, "\n");
}
void CVacationUsed::SetPartiallyPaid(int nPaidDays)
{
	mnPaidDays = nPaidDays;
	mnUnPaidWorkDays = mnWorkDays - mnPaidDays;

	/*
	int nUnpaidToAssign = mnUnPaidWorkDays;
	int nPaidToAssigned = mnPaidDays;

	// Update for relevant month in work period
	CMyTime vacMonth(mFirstDay.mYear,mFirstDay.mMonth,1);
	CMonthInfo *pMonthInfo = gWorkPeriod.GetMonthInfoFor(mFirstDay);
	int nWorkDaysOffPerMonth = gWorkPeriod.CountWorkDaysToEndOfMonthFrom(mFirstDay);
	while (nUnpaidToAssign > 0)
	{
		if (nPaidToAssigned > 0 && nWorkDaysOffPerMonth > 0)
		{
			int nToAssign = min(nPaidToAssigned, nWorkDaysOffPerMonth);
			nPaidToAssigned -= nToAssign;
			nWorkDaysOffPerMonth -= nToAssign;
		}
		if (nUnpaidToAssign > 0 && nWorkDaysOffPerMonth > 0)
		{
			int nToAssign = min(nUnpaidToAssign, nWorkDaysOffPerMonth);
			nUnpaidToAssign -= nToAssign;
			nWorkDaysOffPerMonth -= nToAssign;
			pMonthInfo->SetUnpaid(nToAssign);
		}
		if (nUnpaidToAssign < 1)
			break;
		vacMonth.AddMonth();
		pMonthInfo = gWorkPeriod.GetMonthInfoFor(vacMonth);
		nWorkDaysOffPerMonth = gWorkPeriod.CountWorkDaysToEndOfMonthFrom(vacMonth);
	}*/

	FindUnpaidSpan();
}
void CVacationUsed::UpdateMonthlyInfo()
{
	if (mUnpaidSpan.mnDays > 0)
	{
		mUnpaidSpan.UpdateMonthlyInfo4Unpaid(false); // Unpaid
		mUnpaidSpan.UpdateMonthlyInfo4Unpaid(true); // No pension
	}
}
void CVacationUsed::FindUnpaidSpan()
{
	int n = 0;
	CMyTime check = mFirstDay;
	while (n < mnPaidDays)
	{
		if (check.IsWorkingDay())
			n++;
		check.AddDay();
	}
	if (!mbIsMaternityLeave)
	{
		while (!check.IsWorkingDay())
			check.AddDay();
	}

	mUnpaidSpan.InitDaysSpan(check, mLastDay);
}
int CVacationUsed::CountDaysOfUnpaidVacation(CMyTime& first, CMyTime& last)
{
	if (first > mLastDay)
		return 0;
	if (last < mUnpaidSpan.mFirstDay)
		return 0;

	CMyTime dayAfter = last;
	if (last > mLastDay)
		dayAfter = mLastDay;
	dayAfter.AddDay();

	CMyTime firstToCount = first;
	if (mUnpaidSpan.mFirstDay > first)
		firstToCount = mUnpaidSpan.mFirstDay;

	CTimeSpan span = dayAfter.Subtract(firstToCount);
	return (int)span.GetDays();
}
void CVacationUsed::AddToWorkSpan(CWorkSpan& workSpan)
{
	if (mnUnPaidWorkDays < 1 || mUnpaidSpan.mnDays < 1) 
		// Paid vacation is just like any other work period
		return;

	if (workSpan.Contains(mUnpaidSpan.mFirstDay))
		// Vacation should be added to this span
		workSpan.AddUnpaidVacation(mUnpaidSpan);
}
