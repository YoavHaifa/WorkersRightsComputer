#include "stdafx.h"
#include "PensionReport.h"
#include "HtmlWriter.h"
#include "Pension.h"
#include "FamilyPart.h"


CPensionReportPeriod::CPensionReportPeriod(int year, int month, 
	double monthlyPay, double part, 
	double pensionRate, double severanceRate, 
	double familyPart, double companyHours, double companyRatio)
	: mFromYear(year)
	, mFromMonth(month)
	, mTillYear(year)
	, mTillMonth(month)
	, mMonthlyPay(monthlyPay)
	, mMonthParts(part)
	, mPensionRate(pensionRate)
	, mSeveranceRate(severanceRate)
	, mFamilyPart(familyPart)
	, mCompanyHours(companyHours)
	, mCompanyRatio(companyRatio)
	, mDuePension(0)
	, mDueSeverance(0)
	, mDueFromFamily(0)
{
}
CPensionReportPeriod::~CPensionReportPeriod()
{
}
bool CPensionReportPeriod::Is(double monthlyPay, double pensionRate, double severanceRate, double familyPart)
{
	if (monthlyPay != mMonthlyPay)
		return false;
	if (pensionRate != mPensionRate)
		return false;
	if (severanceRate != mSeveranceRate)
		return false;
	if (familyPart != mFamilyPart)
		return false;

	return true;
}
void CPensionReportPeriod::Add(int year, int month, double part)
{
	mTillYear = year;
	mTillMonth = month;
	mMonthParts += part;
}
void CPensionReportPeriod::WriteToLetter(CHtmlWriter& html, bool bPension)
{
	char zBuf[256];
	sprintf_s(zBuf, 256, "%2d/%4d", mFromMonth, mFromYear);
	html.Write2Tab(zBuf);
	sprintf_s(zBuf, 256, "%2d/%4d", mTillMonth, mTillYear);
	html.Write2Tab(zBuf);

	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		if (mCompanyHours > 0)
			html.Write2Tab("%5.2f", mCompanyHours);
		else if (mCompanyRatio > 0)
			html.Write2Tab("%5.2f%%", mCompanyRatio * 100);
		else
			html.Write2Tab("%d", 0);
	}

	html.Write2Tab(mMonthlyPay);
	html.Write2Tab("%5.3f", mMonthParts);

	double totalPay = mMonthlyPay * mMonthParts;
	html.Write2Tab(totalPay);
	double due = 0;

	if (bPension)
	{
		html.Write2Tab("%5.2f%%", mPensionRate * 100);

		mDuePension = totalPay * mPensionRate;
		html.Write2Tab(mDuePension);
		due = mDuePension;
	}
	else
	{
		html.Write2Tab("%5.2f%%", mSeveranceRate * 100);

		mDueSeverance = totalPay * mSeveranceRate;
		html.Write2Tab(mDueSeverance);
		due = mDueSeverance;
	}

	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		html.Write2Tab("%5.2f%%", mFamilyPart * 100);

		mDueFromFamily = due * mFamilyPart;
		html.Write2Tab(mDueFromFamily);
	}
}

//------------------------------------------------------------------
// class CPensionReport
// --------------------

CPensionReport::CPensionReport()
	: mpLast(NULL)
{
}
CPensionReport::~CPensionReport()
{
	Clear();
}
void CPensionReport::Clear()
{
	while (!mPeriods.IsEmpty())
	{
		delete mPeriods.GetTail();
		mPeriods.RemoveTail();
	}
	mpLast = NULL;
}
void CPensionReport::AddMonth(int year, int month, double monthlyPay, double part, 
	double pensionRate, double severanceRate, double familyPart, double companyHours, double companyRatio)
{
	if (mpLast && mpLast->Is(monthlyPay, pensionRate, severanceRate, familyPart))
		mpLast->Add(year, month, part);
	else
	{
		mpLast = new CPensionReportPeriod(year, month, monthlyPay, part, 
			pensionRate, severanceRate, familyPart, companyHours, companyRatio);
		mPeriods.AddTail(mpLast);
	}
}
void CPensionReport::WriteToLetterPension(CHtmlWriter& html)
{
	double sumPension = 0;
	double sumFromFamily = 0;
	html.StartPensionTable(true, false);
	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		html.StartTabLine();
		CPensionReportPeriod* pPeriod = mPeriods.GetNext(pos);
		pPeriod->WriteToLetter(html, true);
		html.EndTabLine();
		sumPension += pPeriod->mDuePension;
		sumFromFamily += pPeriod->mDueFromFamily;
	}
	// Sum Line
	html.StartTabLine();
	html.Write2TabEH(L"Sum", L"סך הכל");
	html.Write2Tab(" ");
	if (gFamilyPart.mbAskOnlyForFamilyPart)
		html.Write2Tab(" ");
	html.Write2Tab(" ");
	html.Write2Tab(" ");
	html.Write2Tab(" ");
	html.Write2Tab(" ");
	html.Write2Tab(sumPension);
	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		html.Write2Tab(" ");
		html.Write2Tab(sumFromFamily);
	}
	html.EndTabLine();
	html.EndPensionTable();
}
void CPensionReport::WriteToLetterSeverance(CHtmlWriter& html)
{
	double sumSeverance = 0;
	double sumFromFamily = 0;
	html.StartPensionTable(false, true);
	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		html.StartTabLine();
		CPensionReportPeriod* pPeriod = mPeriods.GetNext(pos);
		pPeriod->WriteToLetter(html, false);
		html.EndTabLine();
		sumSeverance += pPeriod->mDueSeverance;
		sumFromFamily += pPeriod->mDueFromFamily;
	}
	// Sum Line
	html.StartTabLine();
	html.Write2TabEH(L"Sum", L"סך הכל");
	html.Write2Tab(" ");
	if (gFamilyPart.mbAskOnlyForFamilyPart)
		html.Write2Tab(" ");
	html.Write2Tab(" ");
	html.Write2Tab(" ");
	html.Write2Tab(" ");
	if (gpPension->mbSeverance)
	{
		html.Write2Tab(" ");
		html.Write2Tab(sumSeverance);
	}
	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		html.Write2Tab(" ");
		html.Write2Tab(sumFromFamily);
	}
	html.EndTabLine();
	html.EndPensionTable();
}
