#include "stdafx.h"
#include "FamilyPart.h"
#include "CompanyPartPeriod.h"
#include "WorkPeriod.h"
#include "Utils.h"
#include "XMLDump.h"
#include "HtmlWriter.h"
#include "XMLParse.h"

CFamilyPart gFamilyPart;

CFamilyPart::CFamilyPart()
	: mbAskOnlyForFamilyPart(false)
	, mRatio(0)
	, msRatio(_T("0"))
	, mbDefined(false)
	, mbLoadingFromXml(false)
{
}
CFamilyPart::CFamilyPart(CFamilyPart& other)
{
	Copy(other);
}
CFamilyPart::~CFamilyPart()
{
	Clear();
}
void CFamilyPart::Clear(void)
{
	while (!mPeriods.IsEmpty())
	{
		delete mPeriods.GetTail();
		mPeriods.RemoveTail();
	}
	mbDefined = false;
	SetRatio(0);
	mbAskOnlyForFamilyPart = false;
}
void CFamilyPart::Copy(CFamilyPart& other)
{
	Clear();

	// Copy all fields
	mbAskOnlyForFamilyPart = other.mbAskOnlyForFamilyPart;
	mRatio = other.mRatio;
	msRatio = other.msRatio;
	mbDefined = other.mbDefined;
	mbLoadingFromXml = false;

	// Copy periods
	POSITION pos = other.mPeriods.GetHeadPosition();
	while (pos)
	{
		CCompanyPartPeriod* pOtherPeriod = other.mPeriods.GetNext(pos);
		mPeriods.AddTail(new CCompanyPartPeriod(*pOtherPeriod));
	}
}
void CFamilyPart::ClearLast(void)
{
	if (!mPeriods.IsEmpty())
	{
		CCompanyPartPeriod *pLast = mPeriods.GetTail();
		if (pLast->mbDummyForApril18)
		{
			if (mPeriods.GetSize() == 2)
			{
				Clear();
				return;
			}
			mPeriods.RemoveTail();
			delete mPeriods.GetTail();
			mPeriods.RemoveTail();
			mPeriods.AddTail(pLast);
		}
		else
		{
			delete pLast;
			mPeriods.RemoveTail();
		}
	}
	if (mPeriods.IsEmpty())
	{
		mbDefined = false;
		SetRatio(0);
		mbAskOnlyForFamilyPart = false;
	}
}
bool CFamilyPart::CheckStartTime(CMyTime& startTime)
{
	if (mPeriods.IsEmpty() && !mbLoadingFromXml)
	{
		if (startTime.mMonth != gWorkPeriod.mFirst.mMonth || startTime.mYear != gWorkPeriod.mFirst.mYear)
		{
			CUtils::MessBox(L"<CFamilyPart::CheckStartTime> First period must start from first day of work", L"Input Error");
			return false;
		}
	}
	if ((startTime.mYear < gWorkPeriod.mFirst.mYear)
		|| (startTime.mYear == gWorkPeriod.mFirst.mYear && startTime.mMonth < gWorkPeriod.mFirst.mMonth))
	{
		CUtils::MessBox(L"<CFamilyPart::CheckStartTime> Period starts before first day of work", L"Input Error");
		return false;
	}
	if ((startTime.mYear > gWorkPeriod.mLast.mYear)
		|| (startTime.mYear == gWorkPeriod.mLast.mYear && startTime.mMonth > gWorkPeriod.mLast.mMonth))
	{
		CUtils::MessBox(L"<CFamilyPart::CheckStartTime> Period starts after last day of work", L"Input Error");
		return false;
	}
	return true;
}
CCompanyPartPeriod* CFamilyPart::GetLastPeriod()
{
	if (mPeriods.IsEmpty())
		return NULL;

	return mPeriods.GetTail();
}
bool CFamilyPart::AddPeriod(CCompanyPartPeriod* pNewPeriod)
{
	CMyTime& startTime = pNewPeriod->mFrom;

	if (!CheckStartTime(startTime))
	{
		delete pNewPeriod;
		return false;
	}

	if (mPeriods.IsEmpty())
	{
		mPeriods.AddTail(pNewPeriod);
		if (!mbDefined)
		{
			mbDefined = true;
			mbAskOnlyForFamilyPart = true;
		}
		CMyTime april18(2018, 4, 1);
		if (startTime < april18 && gWorkPeriod.mLast > april18)
		{
			CCompanyPartPeriod* pDummyPeriod = new CCompanyPartPeriod(april18.mTime);
			mPeriods.AddTail(pDummyPeriod);
		}
		return true;
	}
	
	// Enter new period according to order
	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		POSITION prevPos = pos;
		CCompanyPartPeriod* pPeriod = mPeriods.GetNext(pos);
		if (startTime.IsMonthBefore(pPeriod->mFrom))
		{
			mPeriods.InsertBefore(prevPos, pNewPeriod);
			return true;
		}
		else if (startTime.IsMonthSame(pPeriod->mFrom))
		{
			pPeriod->UpdateFrom(*pNewPeriod);
			return true;
		}
	}

	// Add last
	mPeriods.AddTail(pNewPeriod);
	return true;
}
CString CFamilyPart::GetFullText()
{
	CString s(gWorkPeriod.GetShortSummary());
	s += "\r\n";
	if (mPeriods.IsEmpty())
	{
		s += "Division of hours between company and family was not reported.";
		return s;
	}
	Compute();

	CCompanyPartPeriod *pPrevPeriod = NULL;
	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		CCompanyPartPeriod *pPeriod = mPeriods.GetNext(pos);
		s += pPeriod->GetLine(pPrevPeriod);
		s += "\r\n";
		pPrevPeriod = pPeriod;
	}

	s += "Family Part is ";
	s += msRatio;
	return s;
}
CString CFamilyPart::GetShortText()
{
	if (mPeriods.IsEmpty())
	{
		CString s("Division of hours between company and family was not reported.");
		return s;
	}

	Compute();
	return CString(L"Ask only for Family Part - " + msRatio);
}
void CFamilyPart::Compute()
{
	CCompanyPartPeriod *pPrevPeriod = NULL;
	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		CCompanyPartPeriod *pPeriod = mPeriods.GetNext(pos);
		if (!pPeriod->mbDummyForApril18)
		{
			if (pPrevPeriod)
				gWorkPeriod.SetPartPaidByCompany(pPrevPeriod, pPeriod);
			pPrevPeriod = pPeriod;
		}
	}
	if (pPrevPeriod)
		gWorkPeriod.SetPartPaidByCompany(pPrevPeriod, NULL);

	SetRatio(gWorkPeriod.ComputeFamilyPart());
}
void CFamilyPart::SaveToXml(CXMLDump &xmlDump)
{
	if (mPeriods.IsEmpty())
		return;

	CXMLDumpScope scope(L"FamilyPart", xmlDump);

	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		CCompanyPartPeriod *pPeriod = mPeriods.GetNext(pos);
			pPeriod->SaveToXml(xmlDump);
		{
		}
	}
	xmlDump.Write(L"bAskOnlyForFamilyPart", mbAskOnlyForFamilyPart);
}
void CFamilyPart::LoadFromXml(class CXMLParseNode* pRoot)
{
	Clear();
	CXMLParseNode* pMain = pRoot->GetFirst(L"FamilyPart");
	if (!pMain)
		return;

	mbLoadingFromXml = true;
	CXMLParseNode* pPeriodNode = pMain->GetFirst(L"Period");
	while (pPeriodNode)
	{
		CCompanyPartPeriod* pNewPeriod = new CCompanyPartPeriod(pPeriodNode);
		AddPeriod(pNewPeriod);

		pPeriodNode = pMain->GetNext(L"Period", pPeriodNode);
	}
	pMain->GetValue(L"bAskOnlyForFamilyPart", mbAskOnlyForFamilyPart);
	mbLoadingFromXml = false;
}
void CFamilyPart::WriteToLetter(CHtmlWriter &writer)
{
	writer.StartParagraph();
	if (mbDefined && mbAskOnlyForFamilyPart)
	{
		if (mRatio >= 1)
		{
			writer.WriteLEH(
				L"Private employment - the full amounts due at the end of the employment will be paid by the family. ",
				L"העסקה פרטית – מלוא הסכומים המגיעים עם סיום ההעסקה ישולמו על ידי המשפחה.");
		}
		else
		{
			writer.WriteLEH(L"This computation only relates to the part that is due from the employing family. ",
				L"חישוב זה מתייחס רק לחלק התשלום המגיע מהמשפחה המעסיקה.");

			writer.WriteLEH(L"According to the law, the family and the company are ",
				L"בהתאם להוראות הממונה על עובדים זרים במשרד התעסוקה, המשפחה והחברה הינן ");
			writer.StartBold();
			writer.WriteLEH(L"\"simultaneous employers,\" ", L"מעסיקות במקביל,");
			writer.EndBold();
			writer.WriteLEH(L"and each is responsible for their part of the job and the payment of the rights arising from it.",
				L" וכל אחת אחראית על חלק המשרה שלה ועל תשלום הזכויות הנובעות ממנו.");
		}
	}
	else
	{
		writer.WriteLEH(L"This computation includes the amounts paid by bituakh leumi for severance and pension payment.",
			L"חישוב זה כולל את הסכומים ששולמו על ידי הביטוח הלאומי עבור פיצויים ופנסיה.");
	}
	writer.EndParagraph();
}
void CFamilyPart::SetRatio(double ratio)
{
	mRatio = ratio;
	msRatio = Ratio2S(mRatio);
}
CString CFamilyPart::Ratio2S(double ratio)
{
	wchar_t zBuf[128];
	swprintf_s(zBuf, 128, L"%5.2f%%", ratio * 100);
	return CString(zBuf);
}