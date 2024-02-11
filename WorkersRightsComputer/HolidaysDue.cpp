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
	mHolidaysPerYears.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY,
		IDC_EDIT_HOLIDAYS_PREVY_WORK, IDC_EDIT_HOLIDAYS_PREVY_PAID, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM, IDC_EDIT_HOLIDAYS_PREVY_DUE));
	mHolidaysPerYears.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY1,
		IDC_EDIT_HOLIDAYS_PREVY_WORK1, IDC_EDIT_HOLIDAYS_PREVY_PAID1, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM1, IDC_EDIT_HOLIDAYS_PREVY_DUE1));
	mHolidaysPerYears.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY2,
		IDC_EDIT_HOLIDAYS_PREVY_WORK2, IDC_EDIT_HOLIDAYS_PREVY_PAID2, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM2, IDC_EDIT_HOLIDAYS_PREVY_DUE2));
	mHolidaysPerYears.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY3,
		IDC_EDIT_HOLIDAYS_PREVY_WORK3, IDC_EDIT_HOLIDAYS_PREVY_PAID3, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM3, IDC_EDIT_HOLIDAYS_PREVY_DUE3));
	mHolidaysPerYears.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY4,
		IDC_EDIT_HOLIDAYS_PREVY_WORK4, IDC_EDIT_HOLIDAYS_PREVY_PAID4, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM4, IDC_EDIT_HOLIDAYS_PREVY_DUE4));
	mHolidaysPerYears.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY5,
		IDC_EDIT_HOLIDAYS_PREVY_WORK5, IDC_EDIT_HOLIDAYS_PREVY_PAID5, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM5, IDC_EDIT_HOLIDAYS_PREVY_DUE5));
	mHolidaysPerYears.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY6,
		IDC_EDIT_HOLIDAYS_PREVY_WORK6, IDC_EDIT_HOLIDAYS_PREVY_PAID6, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM6, IDC_EDIT_HOLIDAYS_PREVY_DUE6));
	mHolidaysPerYears.AddTail(new CHolidaysDuePerYear(IDC_STATIC_PY7,
		IDC_EDIT_HOLIDAYS_PREVY_WORK7, IDC_EDIT_HOLIDAYS_PREVY_PAID7, 
		IDC_EDIT_HOLIDAYS_PREVY_FROM7, IDC_EDIT_HOLIDAYS_PREVY_DUE7));

	mpThisYear = mHolidaysPerYears.GetHead();

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
void CHolidaysDue::UpdateMainDialog()
{
	if (!gpDlg)
		return;

	// Should we see holidays at all?
	if (mbPeriodAndHolidaysDefined)
	{
		for (int i = 0; i < N_MAIN_DLG_FIELDS; i++)
			gpDlg->SetVisible(maMainDlgFields[i]);
	}
	else
	{
		for (int i = 0; i < N_MAIN_DLG_FIELDS; i++)
			gpDlg->SetInvisible(maMainDlgFields[i]);
		return;
	}

	// Set content to relevant fields
	mpThisYear->UpdateGui(gpDlg);
	UpdateSum();
	mSumPrev.UpdateShortText(gpDlg, IDC_STATIC_HOLIDAY_PREVS);
}
void CHolidaysDue::OnMainDialogChange(CMyDialogEx* pMainDlg)
{
	mpThisYear->OnGuiChange(pMainDlg);
}
void CHolidaysDue::Reset()
{
	mFirstInPeriod.Reset();
	mLastInPeriod.Reset();
	mbDefinedBySpecialDialog = false;
	mbPeriodAndHolidaysDefined = false;
	msHolidaysSelection = "";

	POSITION pos = mHolidaysPerYears.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYears.GetNext(pos);
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

	SetYearsByWorkPeriod();
}
bool CHolidaysDue::VerifyWorkPeriod(CMyDialogEx* pMainDlg)
{
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
	UpdateMainDialog();
	return true; // Changed
}
void CHolidaysDue::SetYearsByWorkPeriod()
{
	mn = 0;
	int iFromLast = 0;
	POSITION pos = mHolidaysPerYears.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYears.GetNext(pos);
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

	// Init number of holidays in this year
	CHolidays* pHolidays = gAllRights.GetHolidays();
	if (!pHolidays)
		return;
	int nInLast = pHolidays->NInLastYear();
	mpThisYear->SetNInYear(nInLast);

	mSum.mbRelevant = true;
	mSumPrev.mbRelevant = mn > 1;
}
void CHolidaysDue::InitDialog(CMyDialogEx* pDlg)
{
	POSITION pos = mHolidaysPerYears.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYears.GetNext(pos);
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

	POSITION pos = mHolidaysPerYears.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYears.GetNext(pos);
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
	POSITION pos = mHolidaysPerYears.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYears.GetNext(pos);
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

	POSITION pos = mHolidaysPerYears.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYears.GetNext(pos);
		pYear->SaveToXml(xmlDump);
	}
}
void CHolidaysDue::LoadFromXml(CXMLParseNode* pWorkPeriod, CXMLParseNode* pRoot)
{
	SetWorkPeriod();

	CXMLParseNode* pHolidaysDueNode = pWorkPeriod->GetFirst(L"HolidaysDue");
	if (!pHolidaysDueNode)
	{
		LoadFromOldXml(pRoot);
	}
	else
	{
		pHolidaysDueNode->GetValue(L"bDefinedBySpecialDialog", mbDefinedBySpecialDialog);

		CXMLParseNode* pYearNode = pHolidaysDueNode->GetFirst(L"Year");
		POSITION pos = mHolidaysPerYears.GetHeadPosition();
		while (pYearNode && pos)
		{
			CHolidaysDuePerYear* pYear = mHolidaysPerYears.GetNext(pos);
			pYear->LoadFromXml(pYearNode);

			pYearNode = pHolidaysDueNode->GetNext(L"Year", pYearNode);
		}
	}

	UpdateSum();
}
void CHolidaysDue::LoadFromOldXml(CXMLParseNode* pRoot)
{
	CXMLParseNode* pMainDlgNode = pRoot->GetFirstDeep(L"EditBoxes");
	if (!pMainDlgNode)
		return;

	// Init this year
	int nLastWorked = 0;
	int nLastPaid = 0;
	pMainDlgNode->GetValue(L"LastYearWork", nLastWorked);
	pMainDlgNode->GetValue(L"LastYearPaid", nLastPaid);
	mpThisYear->SetValues(nLastWorked, nLastPaid);

	// Init previous years
	int nPrevWorked = 0;
	int nPrevPaid = 0;
	int nPrevYears = 0;
	pMainDlgNode->GetValue(L"PrevYearWork", nPrevWorked);
	pMainDlgNode->GetValue(L"PrevYearPaid", nPrevPaid);
	pMainDlgNode->GetValue(L"PrevNYear", nPrevYears);
	if (nPrevWorked > nPrevPaid)
	{
		mbDefinedBySpecialDialog = true;
		if (nPrevYears == 0)
			nPrevYears = 1;
	}

	POSITION pos = mHolidaysPerYears.GetHeadPosition();
	mHolidaysPerYears.GetNext(pos); // Skip first (which is "this year")
	while (pos && nPrevYears > 0)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYears.GetNext(pos);
		pYear->SetValues(nPrevWorked, nPrevPaid);
		nPrevYears--;
	}
}
int CHolidaysDue::GetNDueLastYear()
{
	if (!mpThisYear)
	{
		CUtils::MessBox(L"<CHolidaysDue::GetNDueLastYear> mpThisYear is NULL", L"SW Error");
		return 0;
	}

	return mpThisYear->GetDue();
}
int CHolidaysDue::GetNPrevYears()
{
	if (mSumPrev.GetDue() == 0)
		return 1;
	return mn;
}
int CHolidaysDue::GetNDuePrevYear(int iWanted)
{
	int i = 0;
	POSITION pos = mHolidaysPerYears.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYears.GetNext(pos);
		if (i == iWanted && pYear->mbRelevant)
			return pYear->GetDue();
		i++;
	}
	return -1;
}
