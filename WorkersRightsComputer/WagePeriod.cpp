#include "stdafx.h"
#include "WagePeriod.h"
#include "XMLDump.h"
#include "XMLParse.h"

const wchar_t* azWageMode[] =
{
	L"WAGE_UNDEF",
	L"WAGE_MIN",
	L"WAGE_MONTHLY",
	L"WAGE_HOURLY",
	L"WAGE_LAST"
};

CWagePeriod::CWagePeriod(EWageMode eMode)
	: meMode(eMode)
	, mMonthlyWage(0)
	, mHourlyWage(0)
	, mHoursPerWeek(0)
{

}
void CWagePeriod::SaveToXml(class CXMLDump& xmlDump)
{
	CXMLDumpScope mainScope(L"Period", xmlDump);
	xmlDump.Write(L"mode", azWageMode[meMode]);
	switch (meMode)
	{
	case WAGE_MONTHLY:
		xmlDump.Write(L"monthly_wage", mMonthlyWage);
		break;
	case WAGE_HOURLY:
		xmlDump.Write(L"hourly_wage", mHourlyWage);
		xmlDump.Write(L"hours_per_week", mHoursPerWeek);
		break;
	default:
		break;
	}
}