#include "stdafx.h"
#include "WagePeriod.h"
#include "WorkPeriod.h"
#include "XMLDump.h"
#include "XMLParse.h"
#include "Right.h"

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
	, mHoursPerMonth(0)
{

}
void CWagePeriod::SetFirst()
{
	mFirst = gWorkPeriod.mFirst;
}
void CWagePeriod::SetLast()
{
	mLast = gWorkPeriod.mLast;
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
		xmlDump.Write(L"hours_per_month", mHoursPerMonth);
		break;
	default:
		break;
	}
}
CString CWagePeriod::GetStateText()
{
	CString s;
	s = mFirst.ToMonthString();
	s += " - ";
	s += mLast.ToMonthString();

	s += ": ";
	s += azWageMode[meMode];
	switch (meMode)
	{
	case WAGE_MONTHLY:
		s += CRight::ToString(mMonthlyWage);
		break;
	case WAGE_HOURLY:
		s += CRight::ToString(mHourlyWage);
		s += " (";
		s += CRight::ToString(mHoursPerMonth);
		s += " hours per month)";
		break;
	default:
		break;
	}
	s += "\r\n";
	return s;
}