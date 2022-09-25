#include "stdafx.h"
#include "Wage.h"
#include "Utils.h"
#include "XMLDump.h"
#include "XMLParse.h"

CWage gWage;

CWage::CWage()
{
	SetMinWage();
}
void CWage::Clear()
{
	while (!mPeriods.IsEmpty())
	{
		delete mPeriods.GetTail();
		mPeriods.RemoveTail();
	}
}
void CWage::SetSingle(EWageMode eMode)
{
	if (mPeriods.GetSize() == 1 && mPeriods.GetHead()->Is(eMode))
		return;
	Clear();
	CWagePeriod* pPeriod = new CWagePeriod(eMode);
	mPeriods.AddTail(pPeriod);
}
void CWage::SetMinWage(void)
{
	SetSingle(WAGE_MIN);
}
void CWage::SetMonthlyWage(double wage)
{
	SetSingle(WAGE_MONTHLY);
	/*
	mbMinWage = false;
	mbMonthlyWage = true;
	mMonthlyWage = wage;
	mHourlyWage = 0;
	mHoursPerWeek = 0;*/
}
void CWage::SetHourlyWage(double wagePerHour, double nHoursPerWeek)
{
	SetSingle(WAGE_HOURLY);
	/*
	mbMinWage = false;
	mbMonthlyWage = false;
	mMonthlyWage = 0;
	mHourlyWage = wagePerHour;
	mHoursPerWeek = nHoursPerWeek;*/
}
bool CWage::IsSinglePeriod(EWageMode& oeMode)
{
	if (mPeriods.GetSize() != 1)
		return false;

	oeMode = mPeriods.GetHead()->Mode();
	return true;
}
void CWage::SaveToXml(CXMLDump& xmlDump)
{
	CXMLDumpScope mainScope(L"WagePeriods", xmlDump);
	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		CWagePeriod* pPeriod = mPeriods.GetNext(pos);
		pPeriod->SaveToXml(xmlDump);
	}
}
void CWage::LoadFromXml(CXMLParseNode* pMain)
{
	CXMLParseNode* pWage = pMain->GetFirst(L"WagePeriods");
	if (!pWage)
	{
		SetMinWage();
		return;
	}
	Clear();
}
/*
void CWage::Restore(FILE* pfRead)
{
	CString s = CUtils::ReadLine(pfRead);

	if (s == "MinWage")
	{
		SetMinWage();
	}
	else if (s == "MonthlyWage")
	{
		double wage = CUtils::ReadFloat(pfRead);
		SetMonthlyWage(wage);
	}
	else
	{
		double wage = CUtils::ReadFloat(pfRead);
		double nHours = CUtils::ReadFloat(pfRead);
		SetHourlyWage(wage, nHours);
	}
}*/
