#include "StdAfx.h"
#include "YearlyRates.h"
#include "Utils.h"
#include "Right.h"
#include "XMLDump.h"

CYearlyRates::CYearlyRates(const wchar_t *zName, int firstYear)
	: msName(zName)
	, mFirstYear(firstYear)
	, mbValid(false)
{
	for (int i = 0; i < MAX_RATES; i++)
	{
		ma[i] = 0;
		mabDefined[i] = false;
	}
	InitFromTextFile();
	PrintLog();
}
double CYearlyRates::RatePerYear(int year)
{
	if (!mbValid)
		return 0;
	int iYear = year - mFirstYear;
	return ma[iYear];
}
bool CYearlyRates::PrintLog(void)
{
	if (!mbValid)
		return false;

	FILE *pfWrite = CUtils::OpenLogFile (msName + L"Rates");
	for (int i = 0; i < MAX_RATES; i++)
	{
		int year = mFirstYear + i;
		CString s = CRight::ToString(year);
		s += "   ";
		s += CRight::ToString(ma[i], 6);
		s += "   ";
		if (mabDefined[i])
			s += "*";
		else
			s += "-";
		fwprintf(pfWrite, L"%s\n", (const wchar_t *)s);
	}
	fclose(pfWrite);
	return true;
}
bool CYearlyRates::InitFromTextFile(void)
{
	CString sfName = L"Rates";
	sfName += msName;
	FILE* pfRead = CUtils::OpenInputFile(sfName);
	if (!pfRead)
		return false;

	for (int i = 0; i < MAX_RATES; i++)
	{
		ma[i] = 0;
		mabDefined[i] = false;
	}

	mnDefs = 0;
	int iLastYear = 0;
	double iLastValue = 0;
	CString sLine = CUtils::ReadLine(pfRead);
	while (sLine.Left(3) != L"end")
	{
		int year = _wtoi(sLine);
		maDefYear[mnDefs] = year;
		int iYear = year - mFirstYear;
		if (iYear < 0)
		{
			CUtils::MessBox(L"Rate Year Too Early", L"Input Error");
			break;
		}

		sLine = CUtils::ReadLine(pfRead);
		if (sLine.Left(3) == L"end")
			break;
		double value = _wtof(sLine);
		maDefRate[mnDefs] = value;
		
		if (iYear < MAX_RATES)
		{
			for (int iFill = iYear; iFill >= 0 && !mabDefined[iFill]; iFill--)
			{
				ma[iFill] = value;
			}
			mabDefined[iYear] = true;
			if (iYear > iLastYear)
			{
				iLastYear = iYear;
				iLastValue = value;
			}
			mnDefs++;
		}
		sLine = CUtils::ReadLine(pfRead);
	}
	fclose(pfRead);

	// Fill Rest
	for (int iFill = MAX_RATES - 1; iFill > 0 && !mabDefined[iFill]; iFill--)
	{
		ma[iFill] = iLastValue;
	}
	mbValid = true;
	return true;
}
void CYearlyRates::SaveMonthlyXmlFile(void)
{
	CString sDirName(CUtils::GetInputPath());
	CXMLDump dump(sDirName, msName, L"MonthlyRates");

	for (int i = 0; i < mnDefs; i++)
	{
		CXMLDumpScope scope(L"rate_start", dump);
		dump.Write(L"year", maDefYear[i]);
		dump.Write(L"month", 1);
		dump.Write(L"rate", maDefRate[i]);
	}
}