#include "StdAfx.h"
#include "MinWage.h"
#include "Holidays.h"
#include "Utils.h"
#include "WorkPeriod.h"

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
