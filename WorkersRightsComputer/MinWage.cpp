#include "StdAfx.h"
#include "MinWage.h"
#include "Holidays.h"
#include "Utils.h"
#include "WorkPeriod.h"
#include "XMLParse.h"
#include "XMLDump.h"
#include "MonthlyRates.h"

CMinWage gMinWage;

CMinWage::CMinWage(void)
	: mpMonthlyRates(NULL)
{
}
bool CMinWage::IsValid(void)
{
	return mpMonthlyRates->mbValid;
}
bool CMinWage::InitFromXmlFile(void)
{
	if (!mpMonthlyRates)
		mpMonthlyRates = new CMonthlyRates(L"MinWage", 1950);

	return mpMonthlyRates->mbValid;
}
/*
bool CMinWage::InitFromTextFile(void)
{
	static bool bFailed = false;
	static bool bRead = false;
	if (bRead)
		return true;

	if (bFailed)
		return false;

	FILE* pfRead = CUtils::OpenInputFile(L"MinWage");

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
	SaveMonthlyXmlFile();
	return true;
}*/
/*
void CMinWage::SaveMonthlyXmlFile(void)
{
	CString sDirName(CUtils::GetInputPath());
	CXMLDump dump(sDirName, L"MinWage", L"MonthlyRates");

	for (int i = mn-1; i >= 0; i--)
	{
		CXMLDumpScope scope(L"rate_start", dump);
		dump.Write(L"year", map[i]->mYear);
		dump.Write(L"month", map[i]->mMonth);
		dump.Write(L"rate", map[i]->mWage);
	}
}*/
/*
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
}*/
/*
double CMinWage::ComputeMonthlyPay(const CMyTime &date)
{
	if (mn < 1)
		return 0;

	for (int i = 0; i < mn; i++)
	{
		if (
			(map[i]->mYear < date.mYear)
			||
			((map[i]->mYear == date.mYear) && (map[i]->mMonth <= date.mMonth))
			)
		{
			return map[i]->mWage;
		}
	}

	return (map[mn - 1]->mWage);
}*/
double CMinWage::GetMonthlyWage(const CMyTime& time)
{
	return mpMonthlyRates->RatePerMonth(time);
	//return ComputeMonthlyPay(time);
}
