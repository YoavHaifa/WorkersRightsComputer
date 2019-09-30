#include "StdAfx.h"
#include "MinWage.h"
#include "Holidays.h"
#include "Utils.h"

CMinWage gMinWage;

CMinWage::CMinWage(void)
{
	InitFromFile();
}
bool CMinWage::IsValid(void)
{
	return mn > 0;
}
bool CMinWage::InitFromFile(void)
{
	static bool bFailed = false;
	static bool bRead = false;
	if (bRead)
		return true;

	if (bFailed)
		return false;

	/*
	CString sfName = L".\\Input\\MinWage.txt";
	if (!CUtils::FileExists(sfName))
	{
		//Directory::SetCurrentDirectory("..\\Release");
		sfName = L"..\\Release\\Input\\MinWage.txt";
		if (!CUtils::FileExists(sfName))
		{
			CString s = L"Failed to find file: ";
			s += sfName;
			MessageBox(NULL,s,L"Installation Error",MB_OK);
			static int nErr = 0;
			nErr++;
			return false;
		}
	} */

	FILE* pfRead = CUtils::OpenInputFile(L"MinWage");
	//FILE* pfRead = CUtils::TryOpenStreamReader(sfName, L"Minimum Wage Inmput");

	mn = CUtils::ReadInt(pfRead);

	map = new CLevel *[mn];
	for (int i = 0; i < mn; i++)
	{
		map[i] = new CLevel;

		map[i]->mYear = CUtils::ReadInt(pfRead);

		map[i]->mMonth = CUtils::ReadInt(pfRead);

		map[i]->mWage = CUtils::ReadFloat(pfRead);
	}

	PrintLog();
	bRead = true;
	return true;
}
void CMinWage::PrintLog(void)
{
	FILE *pfWrite = CUtils::OpenLogFile(L"MinWage");
	if (!pfWrite)
		return;

	fprintf(pfWrite, "n periods defined\n");
	fprintf(pfWrite, "%d\n\n", mn);

	for (int i = 0; i < mn; i++)
	{
		map[i]->Log(pfWrite);
	}
	fclose(pfWrite);
}
double CMinWage::ComputeHolidayPrice(int year, int month)
{
	double baseWage = ComputeHolidayPriceBaseWage(year, month);

	double extraHour = 0;
	if (year > 2016 || (year == 2016 && month > 6))
	{
		extraHour = baseWage / gWorkPeriod.mnWorkingHoursInFullMonth;
	}
	return (baseWage / gWorkPeriod.mnDaysInMonthForDailySalary + extraHour) * 1.5;
}
double CMinWage::ComputeHolidayPriceBaseWage(int year, int month)
{
	for (int i = 0; i < mn; i++)
	{
		if (
			(map[i]->mYear < year)
			|| 
			((map[i]->mYear == year) && (map[i]->mMonth <= month))
			)
		{
			return map[i]->mWage;
		}
	}

	return map[mn-1]->mWage;
}
double CMinWage::PayPerMonthAtWorkEnd(void)
{
	if (mn < 1)
		return 0;

	return ComputeMonthlyPay(gWorkPeriod.mLast.mYear, gWorkPeriod.mLast.mMonth);
}
double CMinWage::PayPerDayAtWorkEnd(void)
{
	if (mn < 1)
		return 0;

	double payPerMonth = PayPerMonthAtWorkEnd();
	return (payPerMonth / gWorkPeriod.mnDaysInMonthForDailySalary);
}

double CMinWage::ComputeMonthlyPay(int year, int month)
{
	if (mn < 1)
		return 0;

	for (int i = 0; i < mn; i++)
	{
		if (
			(map[i]->mYear < year)
			|| 
			((map[i]->mYear == year) && (map[i]->mMonth <= month))
			)
		{
			return map[i]->mWage;
		}
	}

	return (map[mn-1]->mWage);
}
