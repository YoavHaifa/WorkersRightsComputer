#include "stdafx.h"
#include "CompanyPartPeriod.h"
#include "FamilyPartDlg.h"
#include "WorkPeriod.h"
#include "XMLDump.h"
//#include "HtmlWriter.h"
#include "XMLParse.h"

CCompanyPartPeriod::CCompanyPartPeriod(const CFamilyPartDlg& dlg)
	: mFrom(dlg.mStartTime)
	, mHoursPerWeek(dlg.mHoursPerWeek)
	, mPC(dlg.mPC)
	, mFraction(0)
	, mbCompanyPaysMinimumWage(dlg.mbMinimum)
	, mHourlyWage(dlg.mHourlyWage)
	, mbDummyForApril18(false)
{
	mWorkingHoursPerWeek = gWorkPeriod.GetWorkingHoursInFullWeek(mFrom);
	ComputeFraction();
}
CCompanyPartPeriod::CCompanyPartPeriod(class CXMLParseNode* pPeriodNode)
	: mHoursPerWeek(0)
	, mPC(0)
	, mFraction(0)
	, mbCompanyPaysMinimumWage(true)
	, mHourlyWage(0)
	, mbDummyForApril18(false)
{
	LoadFromXml(pPeriodNode);
	mWorkingHoursPerWeek = gWorkPeriod.GetWorkingHoursInFullWeek(mFrom);
}
CCompanyPartPeriod::CCompanyPartPeriod(const CCompanyPartPeriod& other)
	: mFrom(other.mFrom)
	, mHoursPerWeek(other.mHoursPerWeek)
	, mPC(other.mPC)
	, mFraction(other.mFraction)
	, mbCompanyPaysMinimumWage(other.mbCompanyPaysMinimumWage)
	, mHourlyWage(other.mHourlyWage)
	, mbDummyForApril18(other.mbDummyForApril18)
	, mWorkingHoursPerWeek(other.mWorkingHoursPerWeek)
{
}
CCompanyPartPeriod::CCompanyPartPeriod(CTime start)
	: mFrom(start)
	, mHoursPerWeek(0)
	, mPC(0)
	, mFraction(0)
	, mbCompanyPaysMinimumWage(true)
	, mHourlyWage(0)
	, mbDummyForApril18(true)
{
	mWorkingHoursPerWeek = gWorkPeriod.GetWorkingHoursInFullWeek(mFrom);
}
void CCompanyPartPeriod::ComputeFraction()
{
	if (mHoursPerWeek > 0)
		mFraction = mHoursPerWeek / mWorkingHoursPerWeek;
	else
		mFraction = mPC / 100;

	if (mFraction > 1)
		mFraction = 1;
}
void CCompanyPartPeriod::UpdateFrom(CCompanyPartPeriod& other)
{
	mHoursPerWeek = other.mHoursPerWeek;
	mFraction = other.mFraction;
	mbCompanyPaysMinimumWage = other.mbCompanyPaysMinimumWage;
	mHourlyWage = other.mHourlyWage;

	mbDummyForApril18 = false;
}
CString CCompanyPartPeriod::GetLine(CCompanyPartPeriod* pPrev)
{
	char zBuf[128];
	double hours = mHoursPerWeek;
	double part = mFraction;
	if (mbDummyForApril18)
	{
		hours = pPrev->mHoursPerWeek;
		if (hours)
			part = hours / mWorkingHoursPerWeek;
		else
			part = pPrev->mFraction;
	}
	double partPC = part * 100;

	if (hours > 0)
		sprintf_s(zBuf, sizeof(zBuf),
			"From %2d/%4d - %5.2f / %d --> %5.2f%%", mFrom.mMonth, mFrom.mYear, hours, mWorkingHoursPerWeek, partPC);
	else
		sprintf_s(zBuf, sizeof(zBuf),
			"From %2d/%4d --> %5.2f%%", mFrom.mMonth, mFrom.mYear, partPC);
	CString s(zBuf);

	if (!mbCompanyPaysMinimumWage)
	{
		sprintf_s(zBuf, sizeof(zBuf),
			" (company rate per hour %5.2f)", mHourlyWage);
		s += zBuf;
	}

	if (mbDummyForApril18)
	{
		CString ss("[");
		ss += s;
		ss += "]";
		return ss;
	}
	return s;
}
void CCompanyPartPeriod::SaveToXml(CXMLDump& xmlDump)
{
	if (mbDummyForApril18)
		return;

	CXMLDumpScope scope1(L"Period", xmlDump);

	xmlDump.Write(L"From", mFrom);
	if (mHoursPerWeek > 0)
		xmlDump.Write(L"CompanyHoursPerWeek", mHoursPerWeek);
	if (mPC > 0)
		xmlDump.Write(L"CompanyPercent", mPC);
	xmlDump.Write(L"CompanyPart", mFraction);
	xmlDump.Write(L"b_company_pays_minimum", mbCompanyPaysMinimumWage);
	if (mHourlyWage > 0)
		xmlDump.Write(L"CompanyHourlyWage", mHourlyWage);
}
void CCompanyPartPeriod::LoadFromXml(CXMLParseNode* pPeriodNode)
{
	pPeriodNode->GetValue(L"From", mFrom);
	pPeriodNode->GetValue(L"CompanyHoursPerWeek", mHoursPerWeek);
	pPeriodNode->GetValue(L"CompanyPercent", mPC);
	pPeriodNode->GetValue(L"CompanyPart", mFraction);
	pPeriodNode->GetValue(L"b_company_pays_minimum", mbCompanyPaysMinimumWage);
	pPeriodNode->GetValue(L"CompanyHourlyWage", mHourlyWage);
}
