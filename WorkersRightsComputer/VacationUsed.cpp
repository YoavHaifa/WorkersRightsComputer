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
	, mnPaid(0)
	, mnUnPaid(0)
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
		s += CRight::ToString(mnUnPaid);
	}
	return s;
}
void CVacationUsed::Compute()
{
	mnWorkDays = gWorkPeriod.CountWorkingDays(mFirstDay, mLastDay);

	gVacationTable.ComputeNextVacation(*this);
}
void CVacationUsed::ShortLog(FILE *pf, bool bNewLine)
{
	fprintf(pf, "%d/%d/%d - %d/%d/%d",
		mFirstDay.mDay, mFirstDay.mMonth, mFirstDay.mYear, mLastDay.mDay, mLastDay.mMonth, mLastDay.mYear);
	if (bNewLine)
		fprintf(pf, "\n");
}
void CVacationUsed::LongLog(FILE *pf)
{
	fprintf(pf, "%d/%d/%d - %d/%d/%d\n",
		mFirstDay.mDay, mFirstDay.mMonth, mFirstDay.mYear, mLastDay.mDay, mLastDay.mMonth, mLastDay.mYear);
	fprintf(pf, "Work Days %2d / %2d - Paid %2d Unpaid %2d\n",
		mnWorkDays, mnDays, mnPaid, mnUnPaid);
	if (mUnpaidSpan.mFirstDay.mbInitialized)
	{
		fprintf(pf, "Unpaid %d/%d/%d - %d/%d/%d, Unpaid calendar days %d\n",
			mUnpaidSpan.mFirstDay.mDay, mUnpaidSpan.mFirstDay.mMonth, mUnpaidSpan.mFirstDay.mYear, 
			mLastDay.mDay, mLastDay.mMonth, mLastDay.mYear, mUnpaidSpan.mnDays);
	}
	fprintf(pf, "\n");
}
void CVacationUsed::SetPartiallyPaid(int nPaidDays)
{
	mnPaid = nPaidDays;
	mnUnPaid = mnWorkDays - mnPaid;
	int nUnpaidToAssign = mnUnPaid;
	int nPaidToAssigned = mnPaid;

	// Update for relevant month in work period
	CMyTime vacMonth(mFirstDay.mYear,mFirstDay.mMonth,1);
	CMonthInfo *pInfo = gWorkPeriod.GetMonthInfoFor(mFirstDay);
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
			pInfo->SetUnpaid(nToAssign);
		}
		if (nUnpaidToAssign < 1)
			break;
		vacMonth.AddMonth();
		pInfo = gWorkPeriod.GetMonthInfoFor(vacMonth);
		nWorkDaysOffPerMonth = gWorkPeriod.CountWorkDaysToEndOfMonthFrom(vacMonth);
	}

	FindUnpaidSpan();
	//CTimeSpan span = mLastDay.mTime - mUnpaidSpan.mFirstDay.mTime;
	//mUnpaidSpan.mnDays = (int)span.GetDays() + 1;
}
void CVacationUsed::FindUnpaidSpan()
{
	int n = 0;
	CMyTime check = mFirstDay;
	while (n <= mnPaid)
	{
		if (check.IsWorkingDay())
			n++;
		check.AddDay();
	}
	while (!check.IsWorkingDay())
		check.AddDay();

	mUnpaidSpan.InitSpan(check, mLastDay);
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
	if (mnUnPaid < 1 || mUnpaidSpan.mnDays < 1) 
		// Paid vacation is just like any other work period
		return;

	if (workSpan.Contains(mUnpaidSpan.mFirstDay))
		// Vacation should be added to this span
		workSpan.AddUnpaidVacation(mUnpaidSpan);
}
