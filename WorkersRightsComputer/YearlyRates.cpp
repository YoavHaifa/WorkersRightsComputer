#include "StdAfx.h"
#include "YearlyRates.h"
#include "Utils.h"
#include "Right.h"

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
	InitFromFile();
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
bool CYearlyRates::InitFromFile(void)
{
	CString sfName = L"..\\release\\Input\\Rates";
	sfName += msName;
	sfName += L".txt";

	FILE *pfRead = MyFOpenWithErrorBox(sfName, L"r", L"rates input");
	if (!pfRead)
		return false;

	for (int i = 0; i < MAX_RATES; i++)
	{
		ma[i] = 0;
		mabDefined[i] = false;
	}

	int iLastYear = 0;
	double iLastValue = 0;
	CString sLine = CUtils::ReadLine(pfRead);
	while (sLine != L"end")
	{
		int year = _wtoi(sLine);
		year -= mFirstYear;
		if (year < 0)
		{
			CUtils::MessBox(L"Rate Year Too Early", L"Input Error");
			break;
		}

		sLine = CUtils::ReadLine(pfRead);
		if (sLine == L"end")
			break;
		double value = _wtof(sLine);
		
		if (year < MAX_RATES)
		{
			for (int iFill = year; iFill >= 0 && !mabDefined[iFill]; iFill--)
			{
				ma[iFill] = value;
			}
			mabDefined[year] = true;
			if (year > iLastYear)
			{
				iLastYear = year;
				iLastValue = value;
			}
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
