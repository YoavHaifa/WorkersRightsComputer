#include "stdafx.h"
#include "HolidaysDuePerYear.h"
#include "PrevYearsHolidaysDlg.h"
#include "XMLDump.h"
#include "XMLParse.h"
#include "WorkYear.h"
#include "Holidays.h"

int CHolidaysDuePerYear::umId = -2; // First to be constructed is "mSum" - not real year!

//---------------------------------------------------------------------------------------
// struct SHolidayData

void SHolidayData::UpdateGui(CMyDialogEx* pDlg)
{
	pDlg->SetParameter(mGuiId, mValue);
}
void SHolidayData::OnGuiChange(CMyDialogEx* pDlg)
{
	pDlg->GetParameter(mGuiId, mValue);
}
void SHolidayData::SetInvisible(CMyDialogEx* pDlg)
{
	pDlg->SetInvisible(mGuiId);
	mValue = 0;
}
//---------------------------------------------------------------------------------------
// class CHolidaysDuePerYear

CHolidaysDuePerYear::CHolidaysDuePerYear(int idPrompt, int idWorked, int idPaid, int idExist, int idDue)
	: mIdPrompt(idPrompt)
	, mWorked(idWorked)
	, mPaid(idPaid)
	, mInYear(idExist, CHolidays::MAX_HOLIDAYS_PER_YEAR)
	, mDue(idDue)
	, mId(umId++)
{
	mData.AddTail(&mWorked);
	mData.AddTail(&mPaid);
	mData.AddTail(&mInYear);
	mData.AddTail(&mDue);
}
CHolidaysDuePerYear::CHolidaysDuePerYear()
	: mIdPrompt(0)
	, mWorked(0)
	, mPaid(0)
	, mInYear(0)
	, mDue(0)
	, mId(umId++)
{
	mData.AddTail(&mWorked);
	mData.AddTail(&mPaid);
	mData.AddTail(&mInYear);
	mData.AddTail(&mDue);
}
void CHolidaysDuePerYear::Zero()
{
	POSITION pos = mData.GetHeadPosition();
	while (pos)
	{
		SHolidayData* pData = mData.GetNext(pos);
		pData->mValue = 0;
	}
	mnInSum = 0;
}
void CHolidaysDuePerYear::Add(CHolidaysDuePerYear& other)
{
	POSITION pos = mData.GetHeadPosition();
	POSITION opos = other.mData.GetHeadPosition();
	while (pos)
	{
		SHolidayData* pData = mData.GetNext(pos);
		SHolidayData* pOtherData = other.mData.GetNext(opos);
		pData->mValue += pOtherData->mValue;
	}
	mnInSum++;
}
void CHolidaysDuePerYear::Init(CWorkYear* pWorkYear)
{
	// Update Period
	CString s;
	if (mId == 0)
		s = "Last Year: ";
	else
		s.Format(L"Prev Year %d: ", mId);
	s += pWorkYear->mFirstDay.ToHebrewString();
	s += " - ";
	s += pWorkYear->mLastDay.ToHebrewString();
	msPrompt = s;

	Zero();
	mInYear.mValue = CHolidays::MAX_HOLIDAYS_PER_YEAR;
	mbRelevant = true;
}
void CHolidaysDuePerYear::UpdateGui(CMyDialogEx* pDlg)
{
	if (mbRelevant)
	{
		pDlg->SetParameter(mIdPrompt, msPrompt);

		// Set all fields in GUI
		POSITION pos = mData.GetHeadPosition();
		while (pos)
		{
			SHolidayData* pData = mData.GetNext(pos);
			pData->UpdateGui(pDlg);
		}
	}
	else
		SetInvisible(pDlg);
}
void CHolidaysDuePerYear::SetInvisible(CMyDialogEx* pDlg)
{
	pDlg->SetInvisible(mIdPrompt);

	POSITION pos = mData.GetHeadPosition();
	while (pos)
	{
		SHolidayData* pData = mData.GetNext(pos);
		pData->SetInvisible(pDlg);
	}
}
bool CHolidaysDuePerYear::ValidateValues()
{
	bool bChanged = false;
	if (mWorked.mValue > mInYear.mValue)
	{
		mWorked.mValue = mInYear.mValue;
		bChanged = true;
	}
	if (mPaid.mValue > mWorked.mValue)
	{
		mPaid.mValue = mWorked.mValue;
		bChanged = true;
	}
	int due = mWorked.mValue - mPaid.mValue;
	if (due != mDue.mValue)
	{
		mDue.mValue = due;
		bChanged = true;
	}
	return bChanged;
}
void CHolidaysDuePerYear::OnGuiChange(CMyDialogEx* pDlg)
{
	POSITION pos = mData.GetHeadPosition();
	while (pos)
	{
		SHolidayData* pData = mData.GetNext(pos);
		pData->OnGuiChange(pDlg);
	}
	ValidateValues();
	UpdateGui(pDlg);
}
void CHolidaysDuePerYear::SaveToXml(CXMLDump& xmlDump)
{
	CXMLDumpScope scope(L"Year", xmlDump);

	xmlDump.Write(L"id", mId);
	xmlDump.Write(L"Worked", mWorked.mValue);
	xmlDump.Write(L"Paid", mPaid.mValue);
	xmlDump.Write(L"InYear", mInYear.mValue);
}
void CHolidaysDuePerYear::LoadFromXml(CXMLParseNode* pNode)
{
	pNode->GetValue(L"Worked", mWorked.mValue);
	pNode->GetValue(L"Paid", mPaid.mValue);
	pNode->GetValue(L"InYear", mInYear.mValue);
	ValidateValues();
}
void CHolidaysDuePerYear::UpdateShortText(CMyDialogEx* pDlg, int id)
{
	if (mbRelevant && mDue.mValue > 0)
	{
		CString s;
		s.Format(L"Previous %d years holidays: Worked %d, Paid %d, Due %d",
			mnInSum, mWorked.mValue, mPaid.mValue, mDue.mValue);
		pDlg->SetParameter(id, s);
	}
	else
	{
		CString s(L"No holidays defined for previous years");
		pDlg->SetParameter(id, s);
	}
}
void CHolidaysDuePerYear::SetValues(int nWorked, int nPaid)
{
	mWorked.mValue = nWorked;
	mPaid.mValue = nPaid;
	ValidateValues();
}
