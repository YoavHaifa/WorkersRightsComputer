#include "StdAfx.h"
#include "Additional.h"

CAdditional::CAdditional(void)
	: CRight(L"Additional", L"בנוסף")
	, mpDescBox(NULL)
	, mpDueBox(NULL)
{
	miPrintOrder = 6;
	mbSkipIfZero = true;
}
CAdditional::~CAdditional(void)
{
}
bool CAdditional::SetEditRef(class CEditRef *pRef)
{
	if (pRef->msName == "AdditionalDesc")
	{
		mpDescBox = &pRef->mEdit;
		return true;
	}
	if (pRef->msName == "AdditionalSum")
	{
		mpDueBox = &pRef->mEdit;
		return true;
	}
	return false;
}
bool CAdditional::Compute(void)
{
	CString sDue;
	bool bAdditionalSumDefined = GetInputNumber(mpDueBox, sDue, mDuePay);
	if (mDuePay == 0)
		bAdditionalSumDefined = false;

	msToLetter = "";
	CString sDesc;
	mpDescBox->GetWindowTextW(sDesc);
	if (sDesc.GetLength() < 2 && !bAdditionalSumDefined)
	{
		msDue += "No Description";
		msDebug += "Assuming nothing additional, as no description found";
		mDuePay = 0;
		return true;
	}

	if (sDesc.GetLength() < 5)
	{
		msDue += "Missing Description";
		msDebug += "Description of additional issue should be 5 characters at least";
		mDuePay = 0;
		return false;
	}

	if (!bAdditionalSumDefined)
	{
		msDue += "No Paid Sum Defined";
		msDebug += "No Paid Sum Defined";
		return false;
	}

	msDue += sDesc;
	msDue += L" ";
	msDue += ToString(mDuePay);
	msToLetter = sDesc;
	return true;
}
CString CAdditional::GetRightNameForLetter(void)
{
	if (mpDescBox)
	{
		CString sDesc;
		mpDescBox->GetWindowTextW(sDesc);
		return sDesc;
	}
	return msName;
}

