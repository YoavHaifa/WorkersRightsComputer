#include "stdafx.h"
#include "Wage.h"
#include "WagePeriodsDlg.h"
#include "Utils.h"
#include "XMLDump.h"
#include "XMLParse.h"

CWage gWage;

CWage::CWage()
	: mDebug(1)
	, mpFirst(NULL)
	, mpLast(NULL)
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
void CWage::AddPeriod(CWagePeriod* pNewPeriod)
{
	ClearOverlap(*pNewPeriod);

	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		POSITION prevPos = pos;
		CWagePeriod* pPeriod = mPeriods.GetNext(pos);
		if (pPeriod->mLast < pNewPeriod->mFirst)
			continue;
		if (prevPos)
		{
			mPeriods.InsertBefore(prevPos, pNewPeriod);
			return;
		}
	}
	mPeriods.AddTail(pNewPeriod);
}
void CWage::ClearOverlap(CWagePeriod &newPeriod)
{
	// After inserting new period - remove area with older definitions...
	POSITION pos = mPeriods.GetHeadPosition();

	while (pos)
	{
		POSITION prevPos = pos;
		CWagePeriod* pPeriod = mPeriods.GetNext(pos);
		if (pPeriod->mLast.IsMonthBefore(newPeriod.mFirst))
			continue;

		// current period doesn't end before new
		if (pPeriod->mFirst.IsMonthBefore(newPeriod.mFirst))
		{
			// Current period starts before new - it should be shortened or split
			bool bSplit = false;
			if (newPeriod.mLast.IsMonthBefore(pPeriod->mLast))
			{ // Split old period that contains the new
				CWagePeriod* pLaterPeriod = new CWagePeriod(*pPeriod);
				pLaterPeriod->mFirst = newPeriod.mLast.GetMonthAfter();
				if (pos)
					mPeriods.InsertAfter(pos, pLaterPeriod);
				else
					mPeriods.AddTail(pLaterPeriod);
				bSplit = true;
			}
			pPeriod->mLast = newPeriod.mFirst.GetMonthBefore();
			if (bSplit)
				return;
			continue;
		}

		// Current period starts after new
		if (newPeriod.mLast.IsMonthBefore(pPeriod->mFirst))
			return; // Finished. Assume all periods are in clear ascending order

		if (newPeriod.mLast.IsMonthBefore(pPeriod->mLast))
		{
			// Only last part of old period will stay
			pPeriod->mFirst = newPeriod.mLast.GetMonthAfter();
			return;
		}

		mPeriods.RemoveAt(prevPos);
	}
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

	CXMLParseNode* pPeriodNode = pWage->GetFirst(L"Period");
	while (pPeriodNode)
	{
		CWagePeriod* pPeriod = new CWagePeriod(pPeriodNode);
		mPeriods.AddTail(pPeriod);
		pPeriodNode = pWage->GetNext(L"Period", pPeriodNode);
	}
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
		return mPeriods.GetHead()->mnHoursPerMonth;

	CUtils::MessBox(L"Hours per month not well defined", L"SW Error");
	return 0;
}
CString CWage::GetStateDesc()
{
	Update();

	CString s;
	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		CWagePeriod* pPeriod = mPeriods.GetNext(pos);
		s += pPeriod->GetStateTextLine();
	}
	return s;
}
void CWage::Update()
{
	if (mPeriods.IsEmpty())
		SetMinWage();

	mpFirst = mPeriods.GetHead();
	mpFirst->SetFirst();

	mpLast = mPeriods.GetTail();
	mpLast->SetLast();
}