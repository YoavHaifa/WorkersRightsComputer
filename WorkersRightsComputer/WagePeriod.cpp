#include "stdafx.h"
#include "WagePeriod.h"
#include "WorkPeriod.h"
#include "XMLDump.h"
#include "XMLParse.h"
#include "Right.h"
#include "Wage.h"
#include "Utils.h"
#include "MinWage.h"

const wchar_t* azWageMode[] =
{
	L"WAGE_UNDEF",
	L"WAGE_MIN",
	L"WAGE_MONTHLY",
	L"WAGE_HOURLY",
	L"WAGE_LAST"
};

int CWagePeriod::umNewId = 1;

CWagePeriod::CWagePeriod(EWageMode eMode)
	: CDaysSpan(gWorkPeriod.mFirst, gWorkPeriod.mLast)
	, meMode(eMode)
	, mMonthlyWage(0)
	, mHourlyWage(0)
	, mnHoursPerMonth(0)
	, mId(umNewId++)
{
	static int un = 0;
	un++;
}
CWagePeriod::CWagePeriod(EWageMode eMode, const CMyTime& first, const CMyTime& last, double wage, double nHours)
	: CDaysSpan(first, last)
	, meMode(eMode)
	, mMonthlyWage(0)
	, mHourlyWage(0)
	, mnHoursPerMonth(0)
	, mId(umNewId++)
{
	if (meMode == WAGE_MONTHLY)
		mMonthlyWage = wage;
	else if (meMode == WAGE_HOURLY)
	{
		mHourlyWage = wage;
		mnHoursPerMonth = nHours;
	}
}
CWagePeriod::CWagePeriod(const CWagePeriod& other)
	: mId(umNewId++)
{
	Copy(other);
}
CWagePeriod::CWagePeriod(CXMLParseNode* pNode)
	: meMode(WAGE_UNDEF)
	, mMonthlyWage(0)
	, mHourlyWage(0)
	, mnHoursPerMonth(0)
	, mId(umNewId++)
{
	CString sMode;
	bool bError = false;
	CString sError;

	if (pNode->GetValue(L"mode", sMode))
	{
		if (sMode == "WAGE_MIN")
			meMode = WAGE_MIN;
		else if (sMode == "WAGE_MONTHLY")
		{
			meMode = WAGE_MONTHLY;
			pNode->GetValue(L"monthly_wage", mMonthlyWage);
		}
		else if (sMode == "WAGE_HOURLY")
		{
			meMode = WAGE_HOURLY;
			pNode->GetValue(L"hourly_wage", mHourlyWage);
			pNode->GetValue(L"hours_per_month", mnHoursPerMonth);
		}
		else
		{
			bError = true;
			sError = "Unrecognized mode for wage period: ";
			sError += sMode;
		}
	}
	else
	{
		bError = true;
		sError = "Failed to find mode for wage period";
	}

	if (!bError)
	{
		pNode->GetValue(L"first", mFirstDay);
		pNode->GetValue(L"last", mLastDay);
		InitDaysSpan(mFirstDay, mLastDay);
	}

	if (bError)
	{
		CUtils::MessBox(L"Failed to restored saved wage period", L"Save/Restore Error");
	}
}
bool CWagePeriod::IsValid()
{
	return mFirstDay.IsValid() && mLastDay.IsValid();
}
void CWagePeriod::Copy(const CWagePeriod& other)
{
	meMode = other.meMode;
	mFirstDay = other.mFirstDay;
	mLastDay = other.mLastDay;
	mMonthlyWage = other.mMonthlyWage;
	mHourlyWage = other.mHourlyWage;
	mnHoursPerMonth = other.mnHoursPerMonth;
}
void CWagePeriod::SetFirst()
{
	mFirstDay = gWorkPeriod.mFirst;
}
void CWagePeriod::SetLast()
{
	mLastDay = gWorkPeriod.mLast;
}
void CWagePeriod::SaveToXml(class CXMLDump& xmlDump)
{
	CXMLDumpScope mainScope(L"Period", xmlDump);
	xmlDump.Write(L"mode", azWageMode[meMode]);
	xmlDump.Write(L"first", mFirstDay);
	xmlDump.Write(L"last", mLastDay);

	switch (meMode)
	{
	case WAGE_MONTHLY:
		xmlDump.Write(L"monthly_wage", mMonthlyWage);
		break;
	case WAGE_HOURLY:
		xmlDump.Write(L"hourly_wage", mHourlyWage);
		xmlDump.Write(L"hours_per_month", mnHoursPerMonth);
		break;
	default:
		break;
	}
}
CString CWagePeriod::GetSMode()
{
	return CString(azWageMode[meMode]);
}
CString CWagePeriod::GetStateTextLine()
{
	CString s;
	s = mFirstDay.ToMonthString();
	s += " - ";
	s += mLastDay.ToMonthString();

	s += ": ";
	s += azWageMode[meMode];
	s += "  ";
	switch (meMode)
	{
	case WAGE_MONTHLY:
		s += CRight::ToString(mMonthlyWage);
		break;
	case WAGE_HOURLY:
		s += CRight::ToString(mHourlyWage);
		s += " (";
		s += CRight::ToString(mnHoursPerMonth);
		s += " hours per month)";
		break;
	default:
		break;
	}

	if (gWage.mDebug)
	{
		s += " (";
		s += CRight::ToString(mId);
		s += ")";
	}
	s += "\r\n";
	return s;
}
bool CWagePeriod::MayUniteWith(const CWagePeriod& other)
{
	if (meMode != other.meMode)
		return false;
	switch (meMode)
	{
	case WAGE_MIN:
		return true;
	case WAGE_MONTHLY:
		return mMonthlyWage == other.mMonthlyWage;
	case WAGE_HOURLY:
		return (mHourlyWage == other.mHourlyWage)
			&& (mnHoursPerMonth == other.mnHoursPerMonth);
	}
	return false;
}
bool CWagePeriod::ComesJustAfter(const CWagePeriod& other)
{
	CMyTime wantedMonth = other.mLastDay.GetMonthAfter();
	return mFirstDay.IsMonthSame(wantedMonth);
}
bool CWagePeriod::Check()
{
	return mFirstDay <= mLastDay;
}
bool CWagePeriod::GetNextMonth(CMyTime& ioTime)
{
	CMyTime nextMonth = ioTime.GetMonthAfter();
	if (mLastDay.IsMonthBefore(nextMonth))
		return false;
	ioTime = nextMonth;
	return true;
}
double CWagePeriod::GetMonthlyWage(const CMyTime &time)
{
	switch (meMode)
	{
	case WAGE_MIN:
		return gMinWage.GetMonthlyWage(time);
	case WAGE_MONTHLY:
		return mMonthlyWage;
	case WAGE_HOURLY:
		return mHourlyWage * mnHoursPerMonth;
	}
	CUtils::MessBox(L"<CWagePeriod::GetMonthlyWage> bad mode", L"SW Error");
	return 0;
}
double CWagePeriod::GetAgreedMonthlyWage()
{
	switch (meMode)
	{
	case WAGE_MONTHLY:
		return mMonthlyWage;
	case WAGE_HOURLY:
		return mHourlyWage * mnHoursPerMonth;
	default:
		break;
	}
	return 0;
}