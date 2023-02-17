#include "stdafx.h"
#include "HolidaysDue.h"
#include "HolidaysDuePerYear.h"
#include "resource.h"
#include "XMLDump.h"
#include "XMLParse.h"
#include "AllRights.h"
#include "Holidays.h"
#include "WorkYears.h"
#include "Utils.h"

CHolidaysDue gHolidaysDue;

CHolidaysDue::CHolidaysDue()
	: mSum(IDC_STATIC_PY_SUM, IDC_EDIT_HOLIDAYS_PREVY_WORK_SUM,
		IDC_EDIT_HOLIDAYS_PREVY_PAID_SUM, IDC_EDIT_HOLIDAYS_PREVY_FROM_SUM, IDC_EDIT_HOLIDAYS_PREVY_DUE_SUM)
	, mbDefinedBySpecialDialog(false)
{
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY,
		IDC_EDIT_HOLIDAYS_PREVY_WORK, IDC_EDIT_HOLIDAYS_PREVY_PAID, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM, IDC_EDIT_HOLIDAYS_PREVY_DUE));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY1,
		IDC_EDIT_HOLIDAYS_PREVY_WORK1, IDC_EDIT_HOLIDAYS_PREVY_PAID1, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM1, IDC_EDIT_HOLIDAYS_PREVY_DUE1));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY2,
		IDC_EDIT_HOLIDAYS_PREVY_WORK2, IDC_EDIT_HOLIDAYS_PREVY_PAID2, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM2, IDC_EDIT_HOLIDAYS_PREVY_DUE2));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY3,
		IDC_EDIT_HOLIDAYS_PREVY_WORK3, IDC_EDIT_HOLIDAYS_PREVY_PAID3, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM3, IDC_EDIT_HOLIDAYS_PREVY_DUE3));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY4,
		IDC_EDIT_HOLIDAYS_PREVY_WORK4, IDC_EDIT_HOLIDAYS_PREVY_PAID4, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM4, IDC_EDIT_HOLIDAYS_PREVY_DUE4));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY5,
		IDC_EDIT_HOLIDAYS_PREVY_WORK5, IDC_EDIT_HOLIDAYS_PREVY_PAID5, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM5, IDC_EDIT_HOLIDAYS_PREVY_DUE5));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY6,
		IDC_EDIT_HOLIDAYS_PREVY_WORK6, IDC_EDIT_HOLIDAYS_PREVY_PAID6, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM6, IDC_EDIT_HOLIDAYS_PREVY_DUE6));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY7,
		IDC_EDIT_HOLIDAYS_PREVY_WORK7, IDC_EDIT_HOLIDAYS_PREVY_PAID7, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM7, IDC_EDIT_HOLIDAYS_PREVY_DUE7));
}
void CHolidaysDue::Reset()
{
	mFirstInPeriod.Reset();
	mLastInPeriod.Reset();
	mbDefinedBySpecialDialog = false;
	msHolidaysSelection = "";

	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		pYear->Zero();
	}
}
void CHolidaysDue::SetWorkPeriod()
{
	mFirstInPeriod = gWorkPeriod.mFirst;
	mLastInPeriod = gWorkPeriod.mLast;
	CHolidays* pHolidays = gAllRights.GetHolidays();
	if (pHolidays)
	{
		msHolidaysSelection = pHolidays->msSelection;
	}
}
void CHolidaysDue::SetSavedWorkPeriod()
{
	SetWorkPeriod();
	SetYearsByWorkPeriod();
}
void CHolidaysDue::VerifyWorkPeriod()
{
	CHolidays* pHolidays = gAllRights.GetHolidays();
	if (!pHolidays)
		return;

	bool bNewPeriod = true;
	if ((mFirstInPeriod == gWorkPeriod.mFirst)
		&& (mLastInPeriod == gWorkPeriod.mLast))
		bNewPeriod = false;

	if (!bNewPeriod	&& (pHolidays->msSelection == msHolidaysSelection))
		return;

	// New work period - initialize to "undefined"
	if (mbDefinedBySpecialDialog)
	{
		if (bNewPeriod)
		{
			CUtils::MessBox(L"Due to new work period definition - previous years' holidays were reset", L"Warning");
		}
		else
			CUtils::MessBox(L"Due to new holidays selection - previous years' holidays were reset", L"Warning");
	}

	Reset();
	SetWorkPeriod();
	SetYearsByWorkPeriod();
}
void CHolidaysDue::SetYearsByWorkPeriod()
{
	int iFromLast = 0;
	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		CWorkYear* pWorkYear = gWorkYears.GetByReverseIndex(iFromLast);
		if (pWorkYear)
			pYear->Init(pWorkYear);
		else
			pYear->SetIgnore();
		iFromLast++;
	}

	// Init number of holidays in last year
	CHolidays* pHolidays = gAllRights.GetHolidays();
	if (!pHolidays)
		return;
	int nInLast = pHolidays->NInLastYear();
	CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetHead();
	pYear->SetNInYear(nInLast);

	mSum.mbRelevant = true;
}
void CHolidaysDue::InitDialog(CPrevYearsHolidaysDlg* pDlg)
{
	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		pYear->UpdateGui(pDlg);
	}
	UpdateSum(pDlg);
	mbDefinedBySpecialDialog = true;
}
void CHolidaysDue::OnGuiChange(int iYear, CPrevYearsHolidaysDlg* pDlg)
{
	static bool umbHandlingChange = false;
	if (umbHandlingChange)
		return;
	umbHandlingChange = true;

	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		if (pYear->mId == iYear)
		{
			pYear->OnGuiChange(pDlg);
			break;
		}
	}
	UpdateSum(pDlg);
	umbHandlingChange = false;
}
void CHolidaysDue::UpdateSum(CPrevYearsHolidaysDlg* pDlg)
{
	mSum.Zero();
	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		mSum.Add(*pYear);
	}
	mSum.UpdateGui(pDlg);
}
void CHolidaysDue::SaveToXml(CXMLDump& xmlDump)
{
	CXMLDumpScope scope(L"HolidaysDue", xmlDump);

	xmlDump.Write(L"bDefinedBySpecialDialog", mbDefinedBySpecialDialog);

	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		pYear->SaveToXml(xmlDump);
	}
}
void CHolidaysDue::LoadFromXml(class CXMLParseNode* pRoot)
{
	SetSavedWorkPeriod();

	CXMLParseNode* pMain = pRoot->GetFirst(L"HolidaysDue");
	if (!pMain)
		return;

	pMain->GetValue(L"bDefinedBySpecialDialog", mbDefinedBySpecialDialog);

	CXMLParseNode* pYearNode = pMain->GetFirst(L"Year");
	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pYearNode && pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		pYear->LoadFromXml(pYearNode);

		pYearNode = pMain->GetNext(L"Year", pYearNode);
	}
}

