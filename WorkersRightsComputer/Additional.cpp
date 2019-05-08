#include "StdAfx.h"
#include "Additional.h"

CAdditional::CAdditional(void)
	: CRight(L"Additional", L"בנוסף")
	, mpDescBox(NULL)
	, mpDueBox(NULL)
{
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
	CString sDesc;
	mpDescBox->GetWindowTextW(sDesc);
	if (sDesc.GetLength() < 5)
	{
		msDue += "Missing Description";
		msDebug += "Description of additional issue should be 5 characters at least";
		return false;
	}
	
	CString sDue;
	mpDueBox->GetWindowTextW(sDue);
	if (sDue.GetLength() < 1)
	{
		msDebug += "No Due Sum Defined";
		return false;
	}

	mDuePay = _wtof(sDue);
	msDue += sDesc;
	msDue += L" ";
	msDue += ToString(mDuePay);
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

