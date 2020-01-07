#include "StdAfx.h"
#include "MonthlyRates.h"
#include "Utils.h"
#include "Right.h"


CMonthlyRates::CMonthlyRates(const wchar_t *zName, int firstYear)
	: msName(zName)
	, mFirstYear(firstYear)
	, mbValid(false)
{
	InitFromFile();
	PrintLog();
}
double CMonthlyRates::RatePerMonth(int year, int month)
{
	if (!mbValid)
		return 0;
	int iYear = year - mFirstYear;
	if (month < 1)
		return 0;
	month = month - 1;
	return ma[iYear * 12 + month];
}
bool CMonthlyRates::PrintLog(void)
{
	if (!mbValid)
		return false;

	double lastPrinted = 0;

	FILE *pfWrite = CUtils::OpenLogFile(msName + L"Rates");
	for (int i = 0; i < MAX_RATES; i++)
	{
		if (ma[i] != lastPrinted)
		{
			int year = mFirstYear + i / 12;
			int month = (i % 12) + 1;
			CString s = CRight::ToString(year);
			s += L"   ";
			s += CRight::ToString(month);
			s += L"   ";
			s += CRight::ToString(ma[i], 6);
			s += L"   ";
			if (mabDefined[i])
				s += L"*";
			else
				s += "-";
			CRight::WriteLine(pfWrite, (const wchar_t *)s);
			lastPrinted = ma[i];
		}
	}
	fclose(pfWrite);
	return true;
}
bool CMonthlyRates::InitFromFile(void)
{
	/*
	CString sfName = L"..\\release\\Input\\Rates";
	sfName += msName;
	sfName += L".txt";

	FILE *pfRead = MyFOpenWithErrorBox(sfName, L"r", L"input rates"); */
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

	//int iLastIndex = 0;
	//double iLastValue = 0;
	CString sLine;
	sLine = CUtils::ReadLine(pfRead);
	while (sLine.Left(3) != L"end")
	{
		int year = _wtoi(sLine);
		year -= mFirstYear;
		if (year < 0)
		{
			CUtils::MessBox(L"Rate Year Too Early", L"Input Error");
			break;
		}

		sLine = CUtils::ReadLine(pfRead);
		if (sLine.Left(3) == L"end")
			break;
		int month = _wtoi(sLine);
		month -= 1;
		if (month < 0)
		{
			CUtils::MessBox(L"Rate Month Too Small", L"Input Error");
			break;
		}

		sLine = CUtils::ReadLine(pfRead);
		if (sLine.Left(3) == L"end")
			break;
		double value = _wtof(sLine);
		
		int index = year * 12 + month;
		if (index < MAX_RATES)
		{
			for (int iFill = index; iFill < MAX_RATES; iFill++)
			{
				ma[iFill] = value;
			}
			mabDefined[index] = true;
			//if (index > iLastIndex)
			//{
			//	iLastIndex = index;
			//	iLastValue = value;
			//}
		}
		sLine = CUtils::ReadLine(pfRead);
	}
	fclose(pfRead);

	//// Fill Rest
	//for (int iFill = MAX_RATES - 1; iFill > 0 && !mabDefined[iFill]; iFill--)
	//{
	//	ma[iFill] = iLastValue;
	//}
	mbValid = true;
	return true;
}
void CMonthlyRates::CorrectForOldStype(void)
{
	if (!mbValid)
		return;

	for (int i = 0; i < MAX_RATES; i++)
	{
		if (ma[i] == 0.0333)
			ma[i] = 0.033;
	}
}
