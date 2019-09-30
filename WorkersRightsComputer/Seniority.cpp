#include "StdAfx.h"
#include "Seniority.h"
#include "Utils.h"
#include "Right.h"


CSeniority::CSeniority(const wchar_t *zName)
	: msName(zName)
	, mbValid(false)
{
	InitFromFile();
	PrintLog();
}
bool CSeniority::PrintLog(void)
{
	if (!mbValid)
		return false;

	FILE *pfWrite = CUtils::OpenLogFile(L"Seniority" + msName);
	if (!pfWrite)
		return false;

	for (int i = 0; i < MAX_SENIORITY; i++)
	{
		CString s = CRight::ToString(i);
		s += L"   ";
		s += CRight::ToString(ma[i]);
		s += L"   ";
		if (mabDefined[i])
			s += L"*";
		else
			s += L"-";
		fwprintf(pfWrite, L"%s\n", (const wchar_t *)s);
	}
	fclose(pfWrite);
	return true;
}
bool CSeniority::InitFromFile(void)
{
	/*
	CString sfName = L"..\\release\\Input\\Seniority";
	sfName += msName;
	sfName += L".txt";

	FILE *pfRead = MyFOpenWithErrorBox(sfName, L"r", L"Input Defs"); */
	CString sfName = L"Seniority";
	sfName += msName;
	FILE* pfRead = CUtils::OpenInputFile(sfName);
	if (!pfRead)
		return false;

	for (int i = 0; i < MAX_SENIORITY; i++)
	{
		ma[i] = 0;
		mabDefined[i] = false;
	}

	int iLastYear = 0;
	int iLastValue = 0;
	CString sLine;
	sLine = CUtils::ReadLine(pfRead);
	while (sLine != L"end")
	{
		int year = _wtoi(sLine);
		sLine = CUtils::ReadLine(pfRead);
		if (sLine == L"end")
			break;
		int value = _wtoi(sLine);
		
		if (year < MAX_SENIORITY)
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
	for (int iFill = MAX_SENIORITY - 1; iFill > 0 && !mabDefined[iFill]; iFill--)
	{
		ma[iFill] = iLastValue;
	}
	mbValid = true;
	return true;
}
