#include "stdafx.h"
#include "FamilyPart.h"
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
{
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
	if (mPeriods.IsEmpty())
	{
		if (startTime.mMonth != gWorkPeriod.mFirst.mMonth || startTime.mYear != gWorkPeriod.mFirst.mYear)
		{
			CUtils::MessBox(L"First period must startTime from first day of work", L"Input Error");
			return false;
		}
	}
	if ((startTime.mYear < gWorkPeriod.mFirst.mYear)
		|| (startTime.mYear == gWorkPeriod.mFirst.mYear && startTime.mMonth < gWorkPeriod.mFirst.mMonth))
	{
		CUtils::MessBox(L"Period starts before first day of work", L"Input Error");
		return false;
	}
	if ((startTime.mYear > gWorkPeriod.mLast.mYear)
		|| (startTime.mYear == gWorkPeriod.mLast.mYear && startTime.mMonth > gWorkPeriod.mLast.mMonth))
	{
		CUtils::MessBox(L"Period starts after last day of work", L"Input Error");
		return false;
	}
	return true;
}
bool CFamilyPart::AddPeriod(CMyTime& startTime, CCompanyPartPeriod* pNewPeriod)
{
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
			CCompanyPartPeriod* pDummyPeriod = new CCompanyPartPeriod(april18.mTime, 0, 0, true);
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
bool CFamilyPart::AddPeriod(CTime startTime, double hoursPerWeek)
{
	CMyTime start(startTime);
	if (!CheckStartTime(start))
		return false;

	CCompanyPartPeriod *pNewPeriod = new CCompanyPartPeriod(startTime, hoursPerWeek, 0);
	return AddPeriod(start, pNewPeriod);
}
bool CFamilyPart::AddPeriodPC(CTime startTime, double perCentByCompany)
{
	CMyTime start(startTime);
	if (!CheckStartTime(start))
		return false;

	CCompanyPartPeriod *pNewPeriod = new CCompanyPartPeriod(startTime, 0, perCentByCompany);
	return AddPeriod(start, pNewPeriod);
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
				gWorkPeriod.SetWeekDaysPaidByCompany(pPrevPeriod, pPeriod);
			pPrevPeriod = pPeriod;
		}
	}
	if (pPrevPeriod)
		gWorkPeriod.SetWeekDaysPaidByCompany(pPrevPeriod, NULL);

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
		if (!pPeriod->mbDummyForApril18)
		{
			CXMLDumpScope scope1(L"Period", xmlDump);
			xmlDump.Write(L"From", pPeriod->mFrom);
			xmlDump.Write(L"CompanyHoursPerWeek", pPeriod->mCompanyHoursPerWeek);
			xmlDump.Write(L"CompanyPart", pPeriod->mCompanyPart);
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

	CXMLParseNode* pPeriod = pMain->GetFirst(L"Period");
	while (pPeriod)
	{
		CMyTime from;
		double companyHoursPerWeek = 0;
		double companyPart = 0;
		if (pPeriod->GetValue(L"From", from))
		{
			if (pPeriod->GetValue(L"CompanyHoursPerWeek", companyHoursPerWeek))
			{
				if (companyHoursPerWeek > 0)
					AddPeriod(from.mTime, companyHoursPerWeek);
				else if (pPeriod->GetValue(L"CompanyPart", companyPart))
					AddPeriodPC(from.mTime, companyPart*100);
			}
		}
		pPeriod = pMain->GetNext(L"Period", pPeriod);
	}
	pMain->GetValue(L"bAskOnlyForFamilyPart", mbAskOnlyForFamilyPart);
}
void CFamilyPart::Save(FILE *pfSave)
{
	if (mPeriods.IsEmpty())
		return;

	fwprintf(pfSave, L"FamilyPart\n");

	POSITION pos = mPeriods.GetHeadPosition();
	while (pos)
	{
		CCompanyPartPeriod *pPeriod = mPeriods.GetNext(pos);
		if (!pPeriod->mbDummyForApril18)
		{
			fwprintf(pfSave, L"Period\n");
			pPeriod->mFrom.Write(pfSave);
			fwprintf(pfSave, L"%.2f\n", pPeriod->mCompanyHoursPerWeek);
		}
	}
	if (mbAskOnlyForFamilyPart)
		fwprintf(pfSave, L"bAskOnlyForFamilyPart\n");

	fwprintf(pfSave, L"EndFamilyPart\n");
}
/*
void CFamilyPart::Restore(FILE *pfRead)
{
	Clear();
	CString s = CUtils::ReadLine(pfRead);
	while (s == "Period")
	{
		CMyTime from;
		from.Read(pfRead);
		double nHours = CUtils::ReadFloat(pfRead);
		AddPeriod(from.mTime, nHours);
		s = CUtils::ReadLine(pfRead);
	}
	if (s == "bAskOnlyForFamilyPart")
	{
		mbAskOnlyForFamilyPart = true;
		s = CUtils::ReadLine(pfRead); // read last line
	}
}*/
void CFamilyPart::WriteToLetter(CHtmlWriter &writer)
{
	writer.StartParagraph();
	if (mbDefined && mbAskOnlyForFamilyPart)
	{
		writer.WriteLEH(L"This computation only relates to the part that is due from the employing family. ",
			L"חישוב זה מתייחס רק לחלק התשלום המגיע מהמשפחה המעסיקה.");
		writer.StartBold();
		writer.WriteLEH(L"Family Part is ", L"חלקה של המשפחה הוא ");
		//wchar_t zBuf[128];
		//swprintf_s(zBuf, 128, L"%5.2f%%", mRatio * 100);
		writer.WriteL(msRatio);
		writer.EndBold();
		writer.WriteLineEH(L" from the payment for severance, pension and advance notice.", 
			L"מהתשלום עבור פיצויי פיטורים, תגמולי מעסיק והודעה מוקדמת. ");
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
	wchar_t zBuf[128];
	swprintf_s(zBuf, 128, L"%5.2f%%", mRatio * 100);
	msRatio = zBuf;
}