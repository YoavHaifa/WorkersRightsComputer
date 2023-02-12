#include "stdafx.h"
#include "HolidaysDuePerYear.h"
#include "PrevYearsHolidaysDlg.h"
#include "XMLDump.h"
#include "XMLParse.h"

int CHolidaysDuePerYear::umId = -1; // First to be constructed is "mSum" - not real year!

//---------------------------------------------------------------------------------------
// struct SHolidayData

void SHolidayData::UpdateGui(CPrevYearsHolidaysDlg* pDlg)
{
	pDlg->SetParameter(mGuiId, mValue);
}
void SHolidayData::OnGuiChange(CPrevYearsHolidaysDlg* pDlg)
{
	pDlg->GetParameter(mGuiId, mValue);
}

//---------------------------------------------------------------------------------------
// class CHolidaysDuePerYear

CHolidaysDuePerYear::CHolidaysDuePerYear(int idWorked, int idPaid, int idExist, int idDue)
	: mWorked(idWorked)
	, mPaid(idPaid)
	, mInYear(idExist, 9)
	, mDue(idDue)
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
}
void CHolidaysDuePerYear::UpdateGui(CPrevYearsHolidaysDlg* pDlg)
{
	POSITION pos = mData.GetHeadPosition();
	while (pos)
	{
		SHolidayData* pData = mData.GetNext(pos);
		pData->UpdateGui(pDlg);
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
void CHolidaysDuePerYear::OnGuiChange(CPrevYearsHolidaysDlg* pDlg)
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