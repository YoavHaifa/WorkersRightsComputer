#include "stdafx.h"
#include "PensionReport.h"
#include "HtmlWriter.h"
#include "Pension.h"
#include "FamilyPart.h"


CPensionReportPeriod::CPensionReportPeriod(int year, int month, double monthlyPay, double part, double pensionRate, double severanceRate, double familyPart)
	: mFromYear(year)
	, mFromMonth(month)
	, mTillYear(year)
	, mTillMonth(month)
	, mMonthlyPay(monthlyPay)
	, mMonthParts(part)
	, mPensionRate(pensionRate)
	, mSeveranceRate(severanceRate)
	, mFamilyPart(familyPart)
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
void CPensionReportPeriod::WriteToLetter(CHtmlWriter& html)
{
	char zBuf[256];
	sprintf_s(zBuf, 256, "%2d/%4d", mFromMonth, mFromYear);
	html.Write2Tab(zBuf);
	sprintf_s(zBuf, 256, "%2d/%4d", mTillMonth, mTillYear);
	html.Write2Tab(zBuf);

	html.Write2Tab(mMonthlyPay);
	html.Write2Tab("%5.3f", mMonthParts);

	double totalPay = mMonthlyPay * mMonthParts;
	html.Write2Tab(totalPay);

	html.Write2Tab("%5.2f%%", mPensionRate * 100);

	mDuePension = totalPay * mPensionRate;
	html.Write2Tab(mDuePension);

	if (gpPension->mbSeverance)
	{
		html.Write2Tab("%5.2f%%", mSeveranceRate * 100);

		mDueSeverance = totalPay * mSeveranceRate;
		html.Write2Tab(mDueSeverance);

		html.Write2Tab(mDuePension + mDueSeverance);
	}

	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		html.Write2Tab("%5.2f%%", mFamilyPart * 100);
		mDueFromFamily = (mDuePension + mDueSeverance) * mFamilyPart;
		html.Write2Tab(mDueFromFamily);
	}
	/*
	wchar_t zBuf[256];
	swprintf_s(zBuf, 256, L"From %2d/%4d Till %2d/%4d salary %7.2f",
		mFromMonth, mFromYear, mTillMonth, mTillYear, mMonthlyPay);
	CString s(zBuf);
	swprintf_s(zBuf, 256, L" (%5.3f months)", mMonthParts);
	s += zBuf;
	swprintf_s(zBuf, 256, L" %9.2f", totalPay);
	s += zBuf;
	swprintf_s(zBuf, 256, L" Pension %4.2f due %7.2f", mPensionRate * 100, duePension);
	s += zBuf;

	html.WriteLine(s); */
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
void CPensionReport::AddMonth(int year, int month, double monthlyPay, double part, double pensionRate, double severanceRate, double familyPart)
{
	if (mpLast && mpLast->Is(monthlyPay, pensionRate, severanceRate, familyPart))
		mpLast->Add(year, month, part);
	else
	{
		mpLast = new CPensionReportPeriod(year, month, monthlyPay, part, pensionRate, severanceRate, familyPart);
		mPeriods.AddTail(mpLast);
	}
}
void CPensionReport::WriteToLetter(CHtmlWriter& html)
{
	double sumPension = 0;
	double sumSeverance = 0;
	double sumFromFamily = 0;
	html.StartPensionTable();
	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		html.StartTabLine();
		CPensionReportPeriod *pPeriod = mPeriods.GetNext(pos);
		pPeriod->WriteToLetter(html);
		html.EndTabLine();
		sumPension += pPeriod->mDuePension;
		sumSeverance += pPeriod->mDueSeverance;
		sumFromFamily += pPeriod->mDueFromFamily;
	}
	// Sum Line
	html.StartTabLine();
	html.Write2TabEH(L"Sum", L"סך הכל");
	html.Write2Tab(" ");
	html.Write2Tab(" ");
	html.Write2Tab(" ");
	html.Write2Tab(" ");
	html.Write2Tab(" ");
	html.Write2Tab(sumPension);
	if (gpPension->mbSeverance)
	{
		html.Write2Tab(" ");
		html.Write2Tab(sumSeverance);
		html.Write2Tab(sumPension + sumSeverance);
	}
	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		html.Write2Tab(" ");
		html.Write2Tab(sumFromFamily);
	}
	html.EndTabLine();
	html.EndPensionTable();
}
