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
}
void CWage::SetHourlyWage(double wagePerHour, double nHoursPerWeek)
{
	SetSingle(WAGE_HOURLY);
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
double CWage::GetMonthlyWage()
{
	if (mPeriods.GetSize() == 1)
		return mPeriods.GetHead()->mMonthlyWage;

	CUtils::MessBox(L"Monthly wage not well defined", L"SW Error");
	return 0;
}
double CWage::GetHourlyWage()
{
	if (mPeriods.GetSize() == 1)
		return mPeriods.GetHead()->mHourlyWage;

	CUtils::MessBox(L"Hourly wage not well defined", L"SW Error");
	return 0;
}
double CWage::GetHoursPerMonth()
{
	if (mPeriods.GetSize() == 1)
		return mPeriods.GetHead()->mHoursPerMonth;

	CUtils::MessBox(L"Hours per month not well defined", L"SW Error");
	return 0;
}
CString CWage::GetStateText()
{
	Update();

	CString s;
	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		CWagePeriod* pPeriod = mPeriods.GetNext(pos);
		s += pPeriod->GetStateText();
	}
	return s;
}
void CWage::Update()
{
	if (mPeriods.IsEmpty())
		SetMinWage();

	mPeriods.GetHead()->SetFirst();
	mPeriods.GetTail()->SetLast();
}