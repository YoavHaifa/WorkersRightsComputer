#include "stdafx.h"
#include "HolidaysDue.h"
#include "HolidaysDuePerYear.h"
#include "WorkersRightsComputerDlg.h"
#include "resource.h"
#include "XMLDump.h"
#include "XMLParse.h"
#include "AllRights.h"
#include "Holidays.h"
#include "WorkYears.h"
#include "Utils.h"
#include "resource.h"

CHolidaysDue gHolidaysDue;

CHolidaysDue::CHolidaysDue()
	: mSum(IDC_STATIC_PY_SUM, IDC_EDIT_HOLIDAYS_PREVY_WORK_SUM,
		IDC_EDIT_HOLIDAYS_PREVY_PAID_SUM, IDC_EDIT_HOLIDAYS_PREVY_FROM_SUM, IDC_EDIT_HOLIDAYS_PREVY_DUE_SUM)
	, mSumPrev()
	, mbDefinedBySpecialDialog(false)
	, mbPeriodAndHolidaysDefined(false)
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

	mpLastYear = mHolidaysPerYer.GetHead();

	maMainDlgFields[0] = IDC_STATIC_HOLIDAY_LAST;
	maMainDlgFields[1] = IDC_EDIT_HOLIDAYS_PREVY_WORK;
	maMainDlgFields[2] = IDC_STATIC_HL_PAID;
	maMainDlgFields[3] = IDC_EDIT_HOLIDAYS_PREVY_PAID;
	maMainDlgFields[4] = IDC_STATIC_HL_FROM;
	maMainDlgFields[5] = IDC_EDIT_HOLIDAYS_PREVY_FROM;
	maMainDlgFields[6] = IDC_STATIC_HL_DUE;
	maMainDlgFields[7] = IDC_EDIT_HOLIDAYS_PREVY_DUE;
	maMainDlgFields[8] = IDC_STATIC_HOLIDAY_PREVS;
	maMainDlgFields[9] = IDC_BUTTON_PREV_YEARS_HOLIDAYS;
	maMainDlgFields[10] = IDC_CHECK_LIVE_IN;
}
void CHolidaysDue::UpdateMainDialog(CMyDialogEx* pMainDlg)
{
	// Should we see holidays at all?
	if (mbPeriodAndHolidaysDefined)
	{
		for (int i = 0; i < N_MAIN_DLG_FIELDS; i++)
			pMainDlg->SetVisible(maMainDlgFields[i]);
	}
	else
	{
		for (int i = 0; i < N_MAIN_DLG_FIELDS; i++)
			pMainDlg->SetInvisible(maMainDlgFields[i]);
		return;
	}

	// Set content to relevant fields
	mpLastYear->UpdateGui(pMainDlg);
	UpdateSum();
	mSumPrev.UpdateShortText(pMainDlg, IDC_STATIC_HOLIDAY_PREVS);
}
void CHolidaysDue::OnMainDialogChange(CMyDialogEx* pMainDlg)
{
	mpLastYear->OnGuiChange(pMainDlg);
}
void CHolidaysDue::Reset()
{
	mFirstInPeriod.Reset();
	mLastInPeriod.Reset();
	mbDefinedBySpecialDialog = false;
	mbPeriodAndHolidaysDefined = false;
	msHolidaysSelection = "";

	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		pYear->Zero();
	}
	UpdateSum();
}
void CHolidaysDue::SetWorkPeriod()
{
	mFirstInPeriod = gWorkPeriod.mFirst;
	mLastInPeriod = gWorkPeriod.mLast;
	CHolidays* pHolidays = gAllRights.GetHolidays();
	if (pHolidays)
	{
		CString sSelection = pHolidays->GetSelection();
		if (!sSelection.IsEmpty())
		{
			msHolidaysSelection = sSelection;
			mbPeriodAndHolidaysDefined = true;
		}
	}
}
void CHolidaysDue::SetSavedWorkPeriod()
{
	SetWorkPeriod();
	SetYearsByWorkPeriod();
}
bool CHolidaysDue::VerifyWorkPeriod(CMyDialogEx* pMainDlg)
{ // Return "false" if no change
	CHolidays* pHolidays = gAllRights.GetHolidays();
	if (!pHolidays)
		return false;

	bool bNewPeriod = true;
	if ((mFirstInPeriod == gWorkPeriod.mFirst)
		&& (mLastInPeriod == gWorkPeriod.mLast))
		bNewPeriod = false;

	if (!bNewPeriod	&& (pHolidays->Is(msHolidaysSelection)))
		return false;

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
	if (pMainDlg)
		UpdateMainDialog(pMainDlg);
	return true; // Changed
}
void CHolidaysDue::SetYearsByWorkPeriod()
{
	mn = 0;
	int iFromLast = 0;
	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		CWorkYear* pWorkYear = gWorkYears.GetByReverseIndex(iFromLast);
		if (pWorkYear)
		{
			pYear->Init(pWorkYear);
			mn++;
		}
		else
			pYear->SetIgnore();
		iFromLast++;
	}

	// Init number of holidays in last year
	CHolidays* pHolidays = gAllRights.GetHolidays();
	if (!pHolidays)
		return;
	int nInLast = pHolidays->NInLastYear();
	mpLastYear->SetNInYear(nInLast);

	mSum.mbRelevant = true;
	mSumPrev.mbRelevant = mn > 1;
}
void CHolidaysDue::InitDialog(CMyDialogEx* pDlg)
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
void CHolidaysDue::OnGuiChange(int iYear, CMyDialogEx* pDlg)
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
void CHolidaysDue::UpdateSum(CMyDialogEx* pDlg)
{
	mSum.Zero();
	mSumPrev.Zero();
	int i = 0;
	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		mSum.Add(*pYear);
		if (i > 0 && pYear->mbRelevant)
			mSumPrev.Add(*pYear);
		i++;
	}
	if (pDlg)
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

	UpdateSum();
}
int CHolidaysDue::GetNDueLastYear()
{
	if (!mpLastYear)
	{
		CUtils::MessBox(L"<CHolidaysDue::GetNDueLastYear> mpLastYear is NULL", L"SW Error");
		return 0;
	}

	return mpLastYear->GetDue();
}
int CHolidaysDue::GetNPrevYears()
{
	return mn;
}
int CHolidaysDue::GetNDuePrevYear(int iWanted)
{
	int i = 0;
	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		if (i == iWanted && pYear->mbRelevant)
			return pYear->GetDue();
		i++;
	}
	return -1;
}
