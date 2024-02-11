#include "stdafx.h"
#include "VacationDuePerYear.h"
#include "MyDialogEx.h"
#include "WorkYear.h"
#include "WorkYears.h"
#include "WorkPeriod.h"
#include "VacationTable.h"

//---------------------------------------------------------------------------------------
// struct SHolidayData

void SVacationData::UpdateGui(CMyDialogEx* pDlg)
{
	if (mbInt)
	{
		if (msText.IsEmpty())
			pDlg->SetParameter(mGuiId, mValue);
		else
		{
			wchar_t zBuf[128];
			swprintf_s(zBuf, 128, L"%s%d", (const wchar_t *)msText, mValue);
			pDlg->SetParameter(mGuiId, zBuf);
		}
	}
	else if (mbRadio)
		pDlg->SetCheck(mGuiId, mValue!=0);
	else if (!msText.IsEmpty())
		pDlg->SetParameter(mGuiId, msText);
}
void SVacationData::OnGuiChange(CMyDialogEx* pDlg)
{
	if (mbInt)
		pDlg->GetParameter(mGuiId, mValue);
	else if (mbRadio)
	{
		if (pDlg->IsChecked(mGuiId))
			mValue = 1;
		else
			mValue = 0;
	}
}
void SVacationData::SetInvisible(CMyDialogEx* pDlg)
{
	pDlg->SetInvisible(mGuiId);
	mValue = 0;
}
//---------------------------------------------------------------------------------------
// class CHolidaysDuePerYear

int CVacationDuePerYear::umId = 0;

CVacationDuePerYear::CVacationDuePerYear(int idPrompt, int idPaid, int idAll, 
	int idDays, int idDaysPaid, int idAllDays, int idNone,
	int idDueText, int idDaysDue)
	: mPrompt(idPrompt, false, false)
	, mPaid(idPaid, false, false)
	, mAll(idAll, true, false)
	, mDays(idDays, true, false)
	, mDaysPaid(idDaysPaid, false, true)
	, mNAllDays(idAllDays, false, true)
	, mNone(idNone, true, false)
	, mDueText(idDueText, false, false)
	, mDaysDue(idDaysDue, false, true)
	, meVacPY(VACPY_UNDEF)
	, mnPaidDays(0)
	, mnAllDays(0)
	, mnDueDays(0)
	, mbRelevant(false)
	, mId(umId++)
{
	mData.AddTail(&mPrompt);
	mData.AddTail(&mPaid);
	mData.AddTail(&mAll);
	mData.AddTail(&mDays);
	mData.AddTail(&mDaysPaid);
	mData.AddTail(&mNAllDays);
	mData.AddTail(&mNone);
	mData.AddTail(&mDueText);
	mData.AddTail(&mDaysDue);
}
void CVacationDuePerYear::Zero()
{
	POSITION pos = mData.GetHeadPosition();
	while (pos)
	{
		SVacationData* pData = mData.GetNext(pos);
		pData->mValue = 0;
		pData->msText.Empty();
	}
	mAll.mValue = 1;
}
void CVacationDuePerYear::Init(CWorkYear* pWorkYear)
{
	Zero();

	// Update Period
	CString s;
	if (mId == 0)
		s = "This Year: ";
	else
		s.Format(L"Prev Year %d: ", mId);
	s += pWorkYear->mFirstDay.ToHebrewString();
	s += " - ";
	s += pWorkYear->mLastDay.ToHebrewString();
	mPrompt.msText = s;

	CWorkYear* pYear = gWorkYears.GetByReverseIndex(mId);
	double nDaysPerYear = gVacationTable.GetNDaysPerYear(pYear->mSeniority, gWorkPeriod.mnWorkDaysPerWeek);
	mNAllDays.mValue = (int)nDaysPerYear;
	mNAllDays.msText = "/";
	mbRelevant = true;
}
void CVacationDuePerYear::UpdateGui(CMyDialogEx* pDlg)
{
	if (mbRelevant)
	{
		// Set all fields in GUI
		POSITION pos = mData.GetHeadPosition();
		while (pos)
		{
			SVacationData* pData = mData.GetNext(pos);
			pData->UpdateGui(pDlg);
		}
	}
	else
		SetInvisible(pDlg);

}
void CVacationDuePerYear::SetInvisible(CMyDialogEx* pDlg)
{
	POSITION pos = mData.GetHeadPosition();
	while (pos)
	{
		SVacationData* pData = mData.GetNext(pos);
		pData->SetInvisible(pDlg);
	}
}
