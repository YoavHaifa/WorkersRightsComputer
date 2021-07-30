#include "stdafx.h"
#include "Paid.h"

CPaid::CPaid(void)
	: CRight(L"Paid", L"שולם")
	, mpDescBox(NULL)
	, mpDueBox(NULL)
{
	miPrintOrder = 7;
	mbSkipIfZero = true;
}
CPaid::~CPaid(void)
{
}
bool CPaid::SetEditRef(class CEditRef* pRef)
{
	if (pRef->msName == "PaidDesc")
	{
		mpDescBox = &pRef->mEdit;
		return true;
	}
	if (pRef->msName == "PaidSum")
	{
		mpDueBox = &pRef->mEdit;
		return true;
	}
	return false;
}
bool CPaid::Compute(void)
{
	msToLetter = "";
	CString sDesc;
	mpDescBox->GetWindowTextW(sDesc);
	if (sDesc.GetLength() < 5)
	{
		msDue += "Missing Description";
		msDebug += "Description of pay on the account should be 5 characters at least";
		return false;
	}

	CString sDue;
	if (!GetInputNumber(mpDueBox, sDue, mDuePay))
	{
		msDue += "No Paid Sum Defined";
		msDebug += "No Paid Sum Defined";
		return false;
	}

	mDuePay = -mDuePay;
	msDue += sDesc;
	msDue += L" ";
	msDue += ToString(mDuePay);
	msToLetter = sDesc;
	return true;
}
bool CPaid::HasLegalValue()
{
	return mDuePay < 0;
}