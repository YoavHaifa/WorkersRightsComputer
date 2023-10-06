#include "StdAfx.h"
#include "MonthlyRates.h"
#include "Utils.h"
#include "Right.h"
#include "XMLParse.h"
#include "XMLDump.h"

CMonthlyRates::CMonthlyRates(const wchar_t *zName, int firstYear)
	: msName(zName)
	, mFirstYear(firstYear)
	, mbValid(false)
{
	InitFromXmlFile();
}
double CMonthlyRates::RatePerMonth(const CMyTime& date)
{
	if (!mbValid)
		return 0;
	int iYear = date.mYear - mFirstYear;
	if (date.mMonth < 1)
		return 0;
	return ma[iYear * 12 + date.mMonth - 1];
}
bool CMonthlyRates::PrintLog(const wchar_t* zAt)
{
	if (!mbValid)
		return false;

	double lastPrinted = 0;
	CString sfName(msName);
	sfName += L"_Rates_";
	sfName += zAt;

	FILE *pfWrite = CUtils::OpenLogFile(sfName);
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
/*
void CMonthlyRates::SaveMonthlyXmlFile()
{
	CString sDirName(CUtils::GetInputPath());
	CXMLDump dump(sDirName, msName, L"MonthlyRates");

	for (int i = 0; i < mnDefs; i++)
	{
		CXMLDumpScope scope(L"rate_start", dump);
		dump.Write(L"year", maDefYear[i]);
		dump.Write(L"month", maDefMonth[i]);
		dump.Write(L"rate", maDefRate[i]);
	}
}*/
/*
bool CMonthlyRates::InitFromTextFile(void)
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
	CString sLine;
	sLine = CUtils::ReadLine(pfRead);
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
		int month = _wtoi(sLine);
		maDefMonth[mnDefs] = month;
		if (month < 1 || month > 12)
		{
			CUtils::MessBox(L"Wrong Rate Month (should be 1 - 12)", L"Input Error");
			break;
		}

		sLine = CUtils::ReadLine(pfRead);
		if (sLine.Left(3) == L"end")
			break;
		double value = _wtof(sLine);
		maDefRate[mnDefs] = value;
		
		int index = iYear * 12 + (month - 1);
		if (index < MAX_RATES)
		{
			for (int iFill = index; iFill < MAX_RATES; iFill++)
			{
				ma[iFill] = value;
			}
			mabDefined[index] = true;
		}
		mnDefs++;
		sLine = CUtils::ReadLine(pfRead);
	}
	fclose(pfRead);

	mbValid = true;
	PrintLog(L"FromText");
	return true;
}*/
bool CMonthlyRates::InitFromXmlFile(void)
{
	CString sfName(CUtils::GetInputPath());
	sfName += msName;
	sfName += L".MonthlyRates.xml";
	CXMLParse parse(sfName, true /*bUnicode*/);
	CXMLParseNode* pRoot = parse.GetRoot();
	if (!pRoot)
	{
		CUtils::MessBox(parse.GetName(), L"Missing Input File");
		return false;
	}

	for (int i = 0; i < MAX_RATES; i++)
	{
		ma[i] = 0;
		mabDefined[i] = false;
	}

	bool bOK = true;
	CXMLParseNode* pStart = pRoot->GetFirst(L"rate_start");
	while (pStart)
	{
		int year = 0;
		int month = 0;
		double rate = 0;
		if ((pStart->GetValue(L"year", year))
			&& (pStart->GetValue(L"month", month))
			&& (pStart->GetValue(L"rate", rate)))
		{
			int iYear = year - mFirstYear;
			int index = iYear * 12 + (month - 1);
			if (index < MAX_RATES)
			{
				for (int iFill = index; iFill < MAX_RATES; iFill++)
				{
					ma[iFill] = rate;
				}
				mabDefined[index] = true;
			}
		}
		else
		{
			CUtils::MessBox(parse.GetName(), L"Input Error");
			CUtils::MessBox(L"Missing Monthly Rate Details", L"Input Error");
			bOK = false;
			break;
		}
		pStart = pRoot->GetNext(L"rate_start", pStart);
	}

	mbValid = bOK;
	PrintLog(L"FromXML");
	return bOK;
}
