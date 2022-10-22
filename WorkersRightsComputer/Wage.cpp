#include "stdafx.h"
#include "Wage.h"
#include "WageTable.h"
#include "WagePeriodsDlg.h"
#include "Utils.h"
#include "Right.h"
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
void CWage::SetSingle(EWageMode eMode, double wage, double nHours)
{
	Clear();
	CWagePeriod* pPeriod = new CWagePeriod(eMode, 
		gWorkPeriod.mFirst, gWorkPeriod.mLast, wage, nHours);
	mPeriods.AddTail(pPeriod);
}
void CWage::SetMinWage(void)
{
	SetSingle(WAGE_MIN);
}
void CWage::SetMonthlyWage(double wage)
{
	SetSingle(WAGE_MONTHLY, wage);
}
void CWage::SetHourlyWage(double wagePerHour, double nHoursPerWeek)
{
	SetSingle(WAGE_HOURLY, wagePerHour, nHoursPerWeek);
}
bool CWage::IsAllMin()
{
	if (mPeriods.GetSize() < 1)
		return true;
	if (mPeriods.GetSize() > 1)
		return false;
	return mPeriods.GetHead()->Is(WAGE_MIN);
}
bool CWage::IsSinglePeriod(EWageMode& oeMode)
{
	if (mPeriods.GetSize() != 1)
		return false;

	oeMode = mPeriods.GetHead()->Mode();
	return true;
}
bool CWage::AddPeriod(CWagePeriod* pNewPeriod)
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
			return true;
		}
	}
	mPeriods.AddTail(pNewPeriod);
	return true;
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
					mPeriods.InsertBefore(pos, pLaterPeriod);
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

	gWageTable.Prepare(L"LoadFromXml");
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
	UniteAdjacentFileds();

	CString s;
	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		CWagePeriod* pPeriod = mPeriods.GetNext(pos);
		s += pPeriod->GetStateTextLine();
	}
	CheckList();
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
void CWage::UniteAdjacentFileds()
{
	while (mPeriods.GetSize() > 1)
	{
		if (!UniteAdjacentPair())
			break;
	}
}
bool CWage::UniteAdjacentPair()
{
	POSITION pos = mPeriods.GetHeadPosition();
	CWagePeriod* pPrevPeriod = NULL;
	while (pos)
	{
		POSITION prevPos = pos;
		CWagePeriod* pPeriod = mPeriods.GetNext(pos);
		if (pPrevPeriod && pPeriod->MayUniteWith(*pPrevPeriod))
		{
			pPrevPeriod->mLast = pPeriod->mLast;
			mPeriods.RemoveAt(prevPos);
			return true;
		}

		pPrevPeriod = pPeriod;
	}
	return false;
}
bool CWage::CheckList()
{
	POSITION pos = mPeriods.GetHeadPosition();
	CWagePeriod* pPrevPeriod = NULL;
	while (pos)
	{
		CWagePeriod* pPeriod = mPeriods.GetNext(pos);
		if (!pPeriod->Check())
		{
			CUtils::MessBox(L"Illegal Wage Period in list", L"SW Error");
			return false;
		}

		if (pPrevPeriod)
		{
			if (!pPeriod->ComesJustAfter(*pPrevPeriod))
			{
				CUtils::MessBox(L"Illegal Wage Periods pair in list", L"SW Error");
				return false;
			}
		}
		pPrevPeriod = pPeriod;
	}
	return true;
}
CString CWage::GetShortText()
{
	EWageMode eMode;
	if (IsSinglePeriod(eMode))
	{
		CWagePeriod* pPeriod = mPeriods.GetHead();
		CString s;
		switch (eMode)
		{
		case WAGE_MIN:
			return CString("All Minimum Wage");
		case WAGE_MONTHLY:
			s = "Same monthly wage for all period ";
			s += CRight::ToString(pPeriod->mMonthlyWage);
			return s;
		case WAGE_HOURLY:
			s = "Same hourly wage for all period ";
			s += CRight::ToString(pPeriod->mHourlyWage);
			s += " (";
			s += CRight::ToString(pPeriod->mnHoursPerMonth);
			s += " hours per month)";
			return s;
		}
		return CString("Unrecognized wage mode");
	}
	int n = NPeriods();
	CString s;
	s.Format(_T("Wage is defined for %d different periods"), n);
	return s;
}
bool CWage::VerifyWorkPeriod()
{
	bool bShouldCheck = true;
	while (bShouldCheck && mPeriods.GetSize() > 1)
	{
		bShouldCheck = false;
		CWagePeriod* pFirst = mPeriods.GetHead();
		CMyTime lastOfFirst = pFirst->GetLastMonth();
		if (lastOfFirst.IsMonthBefore(gWorkPeriod.mFirst))
		{
			mPeriods.RemoveHead();
			bShouldCheck = true;
		}
		CWagePeriod* pLast = mPeriods.GetTail();
		CMyTime firstOfLast = pLast->GetFirstMonth();
		if (gWorkPeriod.mLast.IsMonthBefore(firstOfLast) && mPeriods.GetSize() > 1)
		{
			mPeriods.RemoveTail();
			bShouldCheck = true;
		}
	}
	Update();
	return gWageTable.Prepare(L"VerifyWorkPeriod");
}
