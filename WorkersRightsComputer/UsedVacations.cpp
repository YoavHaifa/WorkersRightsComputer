#include "stdafx.h"
#include "UsedVacations.h"
#include "Right.h"
#include "Utils.h"
#include "VacationsDlg.h"
#include "VacationUsed.h"
#include "VacationTable.h"
#include "XMLDump.h"
#include "HtmlWriter.h"
#include "XMLParse.h"
#include "WorkSpan.h"
#include "WorkYears.h"
#include "Config.h"

CUsedVacations gUsedVacations;
bool CUsedVacations::umbPrintUnpaid = false;

CUsedVacations::CUsedVacations()
	: mbAdd14DaysUnpaidVacation4Severance(false)
{
}
CUsedVacations::~CUsedVacations()
{
	ClearAllVacations();
}
void CUsedVacations::AddVacation(class CVacationUsed *pNewVacation)
{
	bool bCopyRest = false;
	CVacationUsed *pPrevVac = NULL;
	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		POSITION posPrev = pos;
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);

		if (bCopyRest)
		{
			mVacationsUsed.SetAt(posPrev, pPrevVac);
		}
		else
		{
			if (pNewVacation->mFirstDay < pVac->mFirstDay)
			{
				if (pNewVacation->mLastDay >= pVac->mFirstDay)
				{
					CUtils::MessBox(L"Vacation overlaps with previously defined vacation", L"Input Error");
					delete pNewVacation;
					return;
				}
				if (pPrevVac && pNewVacation->mFirstDay <= pPrevVac->mLastDay)
				{
					CUtils::MessBox(L"Vacation overlaps with previously defined vacation", L"Input Error");
					delete pNewVacation;
					return;
				}
				mVacationsUsed.SetAt(posPrev, pNewVacation);
				bCopyRest = true;
			}
		}

		pPrevVac = pVac;
	}
	if (bCopyRest)
		mVacationsUsed.AddTail(pPrevVac);
	else
	{
		if (pPrevVac && pNewVacation->mFirstDay <= pPrevVac->mLastDay)
		{
			CUtils::MessBox(L"Vacation overlaps with previously defined vacation", L"Input Error");
			delete pNewVacation;
			return;
		}

		mVacationsUsed.AddTail(pNewVacation);
	}
}
void CUsedVacations::ClearLastVacation(void)
{
	if (mVacationsUsed.GetSize() > 0)
	{
		delete mVacationsUsed.GetTail();
		mVacationsUsed.RemoveTail();
	}
}
void CUsedVacations::ClearAllVacations(void)
{
	while (mVacationsUsed.GetSize() > 0)
		ClearLastVacation();

	mbAdd14DaysUnpaidVacation4Severance = gConfig.mb14DaysUnpaidVacation4SeveranceDefault;
}
CString CUsedVacations::GetVacationsShortText(void)
{
	int n = (int)mVacationsUsed.GetSize();
	if (n > 0)
	{
		Compute();
		CString s(CRight::ToString(n));
		s += L" vacations reported";
		return s;
	}
	return CString(L"No Vacations Reported");
}
CString CUsedVacations::GetVacationsFullText(void)
{
	if (mVacationsUsed.GetSize() < 1)
		return CString(L"No Vacations Reported");

	Compute();
	CString s(gWorkPeriod.GetShortSummary());
	s += "\r\n";
	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
		s += pVac->GetText();
		s += L"\r\n";
	}
	return s;
}
void CUsedVacations::SaveToXml(CXMLDump &xmlDump)
{
	if (mVacationsUsed.GetSize() < 1)
		return;

	CXMLDumpScope scope(L"UsedVacations", xmlDump);

	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
		CXMLDumpScope scope1(L"Vacation", xmlDump);
		xmlDump.Write(L"FirstDay", pVac->mFirstDay);
		xmlDump.Write(L"LastDay", pVac->mLastDay);
	}
	xmlDump.Write(L"bAdd14DaysUnpaidVacation4Severance", mbAdd14DaysUnpaidVacation4Severance);
}
void CUsedVacations::LoadFromXml(CXMLParseNode* pRoot)
{
	CXMLParseNode* pMain = pRoot->GetFirst(L"UsedVacations");
	if (!pMain)
		return;

	CXMLParseNode* pVacation = pMain->GetFirst(L"Vacation");
	while (pVacation)
	{
		CMyTime first;
		CMyTime last;
		if (pVacation->GetValue(L"FirstDay", first))
		{
			if (pVacation->GetValue(L"LastDay", last))
			{
				CVacationUsed* pVac = new CVacationUsed(first, last);
				AddVacation(pVac);
			}
		}
		pVacation = pMain->GetNext(L"Vacation", pVacation);
	}
	pMain->GetValue(L"bAdd14DaysUnpaidVacation4Severance", mbAdd14DaysUnpaidVacation4Severance);
}
void CUsedVacations::Save(FILE *pfSave)
{
	if (mVacationsUsed.GetSize() < 1)
		return;

	fwprintf(pfSave, L"Vacations\n");

	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
		fwprintf(pfSave, L"Vacation\n");
		pVac->mFirstDay.Write(pfSave);
		pVac->mLastDay.Write(pfSave);
	}
	if (mbAdd14DaysUnpaidVacation4Severance)
		fprintf(pfSave, "mbAdd14DaysUnpaidVacation4Severance\n");

	fwprintf(pfSave, L"EndVacations\n");
}
void CUsedVacations::Restore(FILE *pfRead)
{
	ClearAllVacations();
	CString s = CUtils::ReadLine(pfRead);
	while (s == "Vacation")
	{
		CMyTime mFirst;
		CMyTime mLast;
		mFirst.Read(pfRead);
		mLast.Read(pfRead);
		CVacationUsed *pVac = new CVacationUsed(mFirst, mLast);
		AddVacation(pVac);
		s = CUtils::ReadLine(pfRead);
	}
	if (s == "mbAdd14DaysUnpaidVacation4Severance")
	{
		mbAdd14DaysUnpaidVacation4Severance = true;
		s = CUtils::ReadLine(pfRead);
	}
	Compute();
}
void CUsedVacations::Compute()
{
	if (!gWorkPeriod.IsValid())
		return;

	gWorkPeriod.InitDetailsForEachMonth();

	gVacationTable.StartComputingForUsedVacations();

	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
		pVac->Compute();
	}
}
void CUsedVacations::UpdateNextYearStart(CMyTime &yearStart, CMyTime &nextYearStart)
{
	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
		if (pVac->mnUnPaid > 0)
		{
			if (pVac->mFirstDayUnpaid >= yearStart && pVac->mFirstDayUnpaid < nextYearStart)
				nextYearStart.AddDays(pVac->mnUnpaidCalendarDays);
		}
	}
}
int CUsedVacations::CountDaysOfUnpaidVacation(CMyTime& first, CMyTime& last)
{
	int nDays = 0;

	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
		if (pVac->mnUnPaid > 0)
		{
			nDays += pVac->CountDaysOfUnpaidVacation(first, last);
		}
	}
	return nDays;
}
void CUsedVacations::AddToWorkSpan(CWorkSpan& workSpan)
{
	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
		if (pVac->mnUnPaid > 0)
		{
			pVac->AddToWorkSpan(workSpan);
		}
	}
}
void CUsedVacations::Log()
{
	FILE *pfLog = CUtils::OpenLogFile(L"UsedVacations");
	if (!pfLog)
		return;
	if (mVacationsUsed.GetSize() < 1)
	{
		fprintf(pfLog, "No vacations defined\n");
	}
	else
	{
		int count = 1;
		POSITION pos = mVacationsUsed.GetHeadPosition();
		while (pos)
		{
			CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
			fwprintf (pfLog, L"%d: ", count++);
			pVac->LongLog(pfLog);
		}
		if (mbAdd14DaysUnpaidVacation4Severance)
			fprintf(pfLog, "mbAdd14DaysUnpaidVacation4Severance\n");
	}
	fclose(pfLog);
}
void CUsedVacations::WriteToLetter(CHtmlWriter& html)
{
	int nVacations = (int)mVacationsUsed.GetSize();
	if (nVacations < 1)
		return;

	html.StartParagraph();
	html.WriteLEH(L"This computation takes into account the following ", L"חישוב זה מביא בחשבון ");
	html.WriteInt(nVacations);
	html.WriteLineEH(L" vacations:", L" חופשות לפי הפרוט הבא:");

	int nPaid = 0;
	int nUnPaid = 0;
	POSITION pos = mVacationsUsed.GetHeadPosition();
	while (pos)
	{
		CVacationUsed *pVac = mVacationsUsed.GetNext(pos);
		html.WriteEH(L"Vacation: ", L"חופשה: ");
		html.WriteEH(pVac->mFirstDay.ToString(), pVac->mFirstDay.ToHebrewString());
		html.Write(L" - ");
		html.WriteEH(pVac->mLastDay.ToString(), pVac->mLastDay.ToHebrewString());

		//fprintf(pf, "Work Days %2d / %2d - Paid %2d Unpaid %2d\n",
		//	mnWorkDays, mnDays, mnPaid, mnUnPaid);
		html.WriteEH(L", work days ", L", ימי עבודה ");
		html.WriteInt(pVac->mnWorkDays);
		html.WriteEH(L", paid ", L", בתשלום ");
		html.WriteInt(pVac->mnPaid);
		html.WriteEH(L", unpaid ", L", ללא תשלום ");
		html.WriteInt(pVac->mnUnPaid);
		nPaid += pVac->mnPaid;
		nUnPaid += pVac->mnUnPaid;

		html.WriteLine(L"");
	}
	html.EndParagraph();

	// Sum of vacations
	html.StartParagraph();
	html.WriteLineEH(L"According to the law, days of paid vacation are included while computing social rights like severance and pension.",
		L"לפי חוק חופשה שנתית, ימי חופשה בתשלום באים בחשבון לצורך חישוב זכויות סוציאליות כגון פיצויי פיטורים והפרשות לפנסיה. ");
	html.WriteEH(L"In this computation ", L"בחישוב זה הוכללו ");
	html.WriteInt(nPaid);
	html.WriteLineEH(L" days of paid vacation were included.", L" ימי חופשה בתשלום.");
	if (umbPrintUnpaid)
	{
		html.WriteEH(L"In this computation ", L"בחישוב זה קוזזו ");
		html.WriteInt(nUnPaid);
		html.WriteLineEH(L" days of unpaid vacation were offset.", L" ימי חופשה ללא תשלום. ");
	}
	html.EndParagraph();
	if (gWorkYears.mnDaysForSeveranceAddedForUnpaidVacations)
		WriteToLetterExtraSeverance(html);
}
void CUsedVacations::WriteToLetterExtraSeverance(class CHtmlWriter& html)
{
	html.StartParagraph();
	html.WriteLineEH(L"According to the law, 14 days of unpaid vacation per year are entitled for severance payment.",
		L"לפי סעיף 10(3) לתקנות פיצויי פיטורים, 14 יום ראשונים של חופשה ללא תשלום בכל שנת עבודה באים בחשבון לצורך חישוב פיצויי הפיטורים.");
	html.WriteEH(L"In this computation ", L"בחישוב זה הוכללו ");
	html.WriteInt(gWorkYears.mnDaysForSeveranceAddedForUnpaidVacations);
	html.WriteLineEH(L" such days were included.", L" ימים כאלו.");
	html.EndParagraph();
}