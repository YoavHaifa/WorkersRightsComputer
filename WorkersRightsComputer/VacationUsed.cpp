#include "stdafx.h"
#include "VacationUsed.h"
#include "Right.h"
#include "WorkPeriod.h"
#include "VacationTable.h"
#include "MonthInfo.h"
#include "WorkYear.h"
#include "XMLDump.h"


CVacationUsed::CVacationUsed(CMyTime &firstDay, CMyTime &lastDay)
	: mFirstDay(firstDay)
	, mLastDay(lastDay)
	, mnDays(0)
	, mnPaid(0)
	, mnUnPaid(0)
	, mnUnpaidCalendarDays(0)
	, mnUnpaidUsedForSeverance(0)
	, mbIsMaternityLeave(false)
{
	mnDays = firstDay.GetNDaysUntil(mLastDay);
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
	if (mFirstDayUnpaid.mbInitialized)
	{
		fprintf(pf, "Unpaid %d/%d/%d - %d/%d/%d, Unpaid calendar days %d\n",
			mFirstDayUnpaid.mDay, mFirstDayUnpaid.mMonth, mFirstDayUnpaid.mYear, 
			mLastDay.mDay, mLastDay.mMonth, mLastDay.mYear, mnUnpaidCalendarDays);
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

	gWorkPeriod.CountNWorkingDaysFrom(mFirstDay, mnPaid, mFirstDayUnpaid);
	CTimeSpan span = mLastDay.mTime - mFirstDayUnpaid.mTime;
	mnUnpaidCalendarDays = (int)span.GetDays() + 1;
}
int CVacationUsed::CountDaysOfUnpaidVacation(CMyTime& first, CMyTime& last)
{
	if (first > mLastDay)
		return 0;
	if (last < mFirstDayUnpaid)
		return 0;

	CMyTime dayAfter = last;
	if (last > mLastDay)
		dayAfter = mLastDay;
	dayAfter.AddDay();

	CMyTime firstToCount = first;
	if (mFirstDayUnpaid > first)
		firstToCount = mFirstDayUnpaid;

	CTimeSpan span = dayAfter.Subtract(firstToCount);
	return (int)span.GetDays();
}
void CVacationUsed::AddToWorkSpan(CWorkSpan& workSpan)
{
	if (mnUnPaid < 1) 
		// Paid vacation is just like any other work period
		return;

	if (mFirstDayUnpaid > workSpan.mLastDay) 
		// Relevant unpaid vacation is after span
		return;

	if (mLastDay < workSpan.mFirstDay)
		// Relevant unpaid vacation is before span
		return;

	// Vacation is relevant for this span
	workSpan.AddUnpaidVacation(*this);
}
