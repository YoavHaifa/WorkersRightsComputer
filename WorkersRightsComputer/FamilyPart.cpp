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
	mRatio = 0;
	mbAskOnlyForFamilyPart = false;
}
bool CFamilyPart::AddPeriod(CTime startTime, double hoursPerWeek)
{
	CMyTime start(startTime);
	if (mPeriods.IsEmpty())
	{
		if (start.mMonth != gWorkPeriod.mFirst.mMonth || start.mYear != gWorkPeriod.mFirst.mYear)
		{
			CUtils::MessBox(L"First period must start from first day of work", L"Input Error");
			return false;
		}
	}
	if ((start.mYear < gWorkPeriod.mFirst.mYear)
		|| (start.mYear == gWorkPeriod.mFirst.mYear && start.mMonth < gWorkPeriod.mFirst.mMonth))
	{
		CUtils::MessBox(L"Period starts before first day of work", L"Input Error");
		return false;
	}
	if ((start.mYear > gWorkPeriod.mLast.mYear)
		|| (start.mYear == gWorkPeriod.mLast.mYear && start.mMonth > gWorkPeriod.mLast.mMonth))
	{
		CUtils::MessBox(L"Period starts after last day of work", L"Input Error");
		return false;
	}

	if (mPeriods.IsEmpty())
	{
		CCompanyPartPeriod *pPeriod = new CCompanyPartPeriod(startTime, hoursPerWeek);
		mPeriods.AddTail(pPeriod);
		if (!mbDefined)
		{
			mbDefined = true;
			mbAskOnlyForFamilyPart = true;
		}
		CMyTime april18(2018, 4, 1);
		CCompanyPartPeriod *pDummyPeriod = new CCompanyPartPeriod(april18.mTime, 0, true);
		mPeriods.AddTail(pDummyPeriod);
	}
	else // Enter new period according to order
	{
		bool bFound = false;
		POSITION pos = mPeriods.GetHeadPosition();
		while (pos)
		{
			POSITION prevPos = pos;
			CCompanyPartPeriod *pPeriod = mPeriods.GetNext(pos);
			if (start.IsMonthBefore(pPeriod->mFrom))
			{
				mPeriods.InsertBefore(prevPos, new CCompanyPartPeriod(startTime, hoursPerWeek));
				bFound = true;
				break;
			}
			else if (start.IsMonthSame(pPeriod->mFrom))
			{
				pPeriod->UpdateHours(hoursPerWeek);
				bFound = true;
				break;
			}
		}
		if (!bFound)
			mPeriods.AddTail(new CCompanyPartPeriod(startTime, hoursPerWeek));
	}

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

	char zBuf[128];
	sprintf_s(zBuf, sizeof(zBuf), "Family Part is %5.2f%%", mRatio * 100);
	s += zBuf;
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

	char zBuf[128];
	sprintf_s(zBuf, sizeof(zBuf), "Ask only for Family Part - %5.2f%%", mRatio * 100);
	return CString(zBuf);
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

	mRatio = gWorkPeriod.ComputeFamilyPart();
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
		}
	}
	xmlDump.Write(L"bAskOnlyForFamilyPart", mbAskOnlyForFamilyPart);
}
void CFamilyPart::LoadFromXml(class CXMLParseNode* pRoot)
{
	CXMLParseNode* pMain = pRoot->GetFirst(L"FamilyPart");
	if (!pMain)
		return;

	CXMLParseNode* pPeriod = pMain->GetFirst(L"Period");
	while (pPeriod)
	{
		CMyTime from;
		double companyHoursPerWeek;
		if (pPeriod->GetValue(L"From", from))
		{
			if (pPeriod->GetValue(L"CompanyHoursPerWeek", companyHoursPerWeek))
			{
				AddPeriod(from.mTime, companyHoursPerWeek);
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
void CFamilyPart::Restore(FILE *pfRead)
{
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
}
void CFamilyPart::WriteToLetter(CHtmlWriter &writer)
{
	writer.StartParagraph();
	if (mbDefined && mbAskOnlyForFamilyPart)
	{
		writer.WriteL(L"This computation only relates to the part that is due from the employing family.");
		fwprintf(writer.mpfWrite, L"Family Part is %5.2f%%\n", mRatio * 100);
	}
	else
	{
		writer.WriteL(L"This computation includes the amounts paid by bituakh leumi for severance and pension payment.");
	}
	writer.EndParagraph();
}
