#include "stdafx.h"
#include "WorkYears.h"
#include "WorkYear.h"
#include "WorkPeriod.h"
#include "Utils.h"

CWorkYears gWorkYears;

CWorkYears::CWorkYears(void)
	: mn(0)
{
}
void CWorkYears::Clear(void)
{
	mn = 0;
}
void CWorkYears::Compute(void)
{
	mn = 0;
	if (!gWorkPeriod.IsValid())
		return;

	maYears[0].InitFirst();
	mn++;
	while (!maYears[mn - 1].mbLast)
	{
		maYears[mn].InitNext(maYears[mn - 1]);
		mn++;
	}
}

void CWorkYears::Log()
{
	FILE* pfLog = CUtils::OpenLogFile(L"WorkYears");
	if (!pfLog)
		return;
	if (mn < 1)
	{
		fprintf(pfLog, "No work years defined\n");
	}
	else
	{
		fprintf(pfLog, "%d work years defined\n\n", mn);
		for (int i = 0; i < mn; i++)
		{
			maYears[i].Log(pfLog);
		}
	}
	fclose(pfLog);
}
