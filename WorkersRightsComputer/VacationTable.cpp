#include "stdafx.h"
#include "VacationTable.h"
#include "Utils.h"
#include "Right.h"
#include "WorkPeriod.h"
#include "VacationUsed.h"

CVacationTable gVacationTable;

CVacationTable::CVacationTable()
	: mnVacationsComputed(0)
	, mpfLog(0)
{
	if (InitFromFile())
		PrintLog();
}


CVacationTable::~CVacationTable()
{
}
bool CVacationTable::InitFromFile(void)
{
	// CString sDir = Directory::GetCurrentDirectory();
	FILE* pfRead = CUtils::OpenInputFile(L"Vacation");

	//CString sfName = L"..\\release\\Input\\Vacation.txt";

	//FILE *pfRead = MyFOpenWithErrorBox(sfName, L"r", L"Vacation Data");
	if (!pfRead)
		return false;

	CString sLine;
	mn = 0;

	for (int i = 0; i < MAX_VETEK; i++)
	{
		sLine = CUtils::TryReadLine(pfRead);
		if (sLine.IsEmpty())
			break;
		int n = _wtoi(sLine);
		if (n < 1)
			break;

		map[i] = new CVacationVetek;
		map[i]->mYears = n;

		sLine = CUtils::ReadLine(pfRead);
		map[i]->mFromYear = _wtoi(sLine);

		sLine = CUtils::ReadLine(pfRead);
		map[i]->mFromMonth = _wtoi(sLine);

		sLine = CUtils::ReadLine(pfRead);
		map[i]->m6 = _wtoi(sLine);

		sLine = CUtils::ReadLine(pfRead);
		map[i]->m5 = _wtoi(sLine);

		mn++;
	}
	return true;
}
bool CVacationTable::PrintLog(void)
{
	if (mn < 1)
		return false;

	FILE *pfLog = CUtils::OpenLogFile(L"Rates_VacationTable");
	if (!pfLog)
		return false;

	for (int i = 0; i < mn; i++)
	{
		CString s = CRight::ToString(map[i]->mYears);
		s += L"   ";
		s += CRight::ToString(map[i]->m6);
		s += L"   ";
		s += CRight::ToString(map[i]->m5);
		s += "   ";
		if (map[i]->mFromYear > 0)
		{
			s += L"from ";
			s += CRight::ToString(map[i]->mFromYear);
			s += L"   ";
			s += CRight::ToString(map[i]->mFromMonth);
		}
		fwprintf(pfLog, L"%s\n", (const wchar_t *)s);
	}
	fclose(pfLog);
	return true;
}
double CVacationTable::GetNDaysPerMonth(int seniority, double nDaysPerWeek, int year, int month)
{
	if (mn < 1)
		return 0;

	double nDays = 0;
	int seniorityUsed = 1000;
	for (int i = 0; i < mn; i++)
	{
		//LogLine(L"<GetNDaysPerMonth> i", i);
		//LogLine(L"map[i]->mYears", map[i]->mYears);
		if (map[i]->mYears > seniorityUsed)
			break;
		if (seniority <= map[i]->mYears)
		{
			//LogLine(L"<GetNDaysPerMonth> i", i);
			if (year > map[i]->mFromYear
				|| (year == map[i]->mFromYear && month >= map[i]->mFromMonth))
			{
				if (nDaysPerWeek > 5)
					nDays = map[i]->m6 / 12.0;
				else
					nDays = map[i]->m5 / 12.0;
				seniorityUsed = map[i]->mYears;
			}
		}
	}
	if (nDays > 0)
		return nDays;

	// No entry - Use Last
	if (nDaysPerWeek > 5)
		return map[mn - 1]->m6;

	return map[mn - 1]->m5;
}
bool CVacationTable::StartComputingForUsedVacations()
{
	gWorkPeriod.Debug(L"CVacationTable::StartComputingForUsedVacations");

	mnVacationsComputed = 0;
	if (!gWorkPeriod.IsValid())
		return false;

	mDueVacationLeft = 0;
	mNextComputeFrom = gWorkPeriod.mFirst;

	return true;
}


bool CVacationTable::ComputeNextVacation(CVacationUsed &vacation)
{
	if (!gWorkPeriod.IsValid())
	{
		vacation.mnPaid = 0;
		return false;
	}

	gWorkPeriod.Debug(L"CVacationTable::ComputeNextVacation");
	mpfLog = CUtils::OpenLogFile(L"ComputeVacations", mnVacationsComputed > 0);
	if (mpfLog && mnVacationsComputed == 0)
	{
		CString sPeriod = gWorkPeriod.GetShortSummary();
		fwprintf(mpfLog, L"%s\n\n", (const wchar_t *)sPeriod);
	}
	if (mpfLog)
	{ 
		fprintf(mpfLog, "<ComputeNextVacation> %d\n", mnVacationsComputed);
		vacation.ShortLog(mpfLog);
	}

	if (!mNextComputeFrom.IsOnPrevMonthsTo(vacation.mFirstDay))
	{
		if (mpfLog)
			fprintf(mpfLog, "No new month to compute vacation - due vacation %.2f", mDueVacationLeft);
	}
	else
	{
		gWorkPeriod.Debug(L"CVacationTable::ComputeNextVacation 1");
		while (mNextComputeFrom.IsOnPrevMonthsTo(vacation.mFirstDay))
		{
			if (mpfLog)
				fprintf(mpfLog, "%2d/%4d - ", mNextComputeFrom.mMonth, mNextComputeFrom.mYear);
			CMonthInfo *pMonthInfo = gWorkPeriod.GetMonthInfoFor(mNextComputeFrom);
			double duePerMonth = GetNDaysPerMonth(pMonthInfo->mSeniorityMonths / 12,
				gWorkPeriod.mnWorkDaysPerWeek, mNextComputeFrom.mYear, mNextComputeFrom.mMonth);

			gWorkPeriod.Debug(L"CVacationTable::ComputeNextVacation 3");
			if (pMonthInfo->IsPartial())
			{
				double dueFrac = duePerMonth * pMonthInfo->mFraction;
				mDueVacationLeft += dueFrac;
				if (mpfLog)
					fprintf(mpfLog, "due %.2f * fraction %.2f --> %.2f --> %.2f\n", duePerMonth, pMonthInfo->mFraction, dueFrac, mDueVacationLeft);
			}
			else
			{
				mDueVacationLeft += duePerMonth;
				if (mpfLog)
					fprintf(mpfLog, "due %.2f --> %.2f\n", duePerMonth, mDueVacationLeft);
			}
			gWorkPeriod.Debug(L"CVacationTable::ComputeNextVacation 5");
			mNextComputeFrom.AddMonth();
		}
	}
	gWorkPeriod.Debug(L"CVacationTable::ComputeNextVacation 7");
	if (mDueVacationLeft > vacation.mnWorkDays)
		vacation.mnPaid = vacation.mnWorkDays;
	else
		vacation.SetPartiallyPaid((int)mDueVacationLeft);

	gWorkPeriod.Debug(L"CVacationTable::ComputeNextVacation 9");
	mDueVacationLeft -= vacation.mnPaid;
	vacation.LongLog(mpfLog);

	mnVacationsComputed++;
	fprintf(mpfLog, "Left Vacation %.2f\n", mDueVacationLeft);
	fprintf(mpfLog, "\n");
	fclose(mpfLog);
	gWorkPeriod.Debug(L"CVacationTable::ComputeNextVacation END");
	return true;
}
