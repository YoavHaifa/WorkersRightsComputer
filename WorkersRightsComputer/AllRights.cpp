#include "stdafx.h"
#include "AllRights.h"
#include "Notice.h"
#include "Recuperation.h"
#include "Severance.h"
#include "Vacation.h"
#include "Holidays.h"
#include "Pension.h"
#include "Additional.h"
#include "Paid.h"
#include "Utils.h"
#include "WorkPeriod.h"
#include "WageTable.h"
#include "WorkersRightsComputerDlg.h"
#include "LogoWriter.h"
#include "HtmlWriter.h"
#include "Person.h"
#include "XMLDump.h"
#include "MinWage.h"

CAllRights gAllRights;

CAllRights::CAllRights()
	: mpHolidays(NULL)
	, mbComputedOK(false)
{
}
CAllRights::~CAllRights()
{
}
bool CAllRights::Init()
{
	Clear();

	gMinWage.InitFromFile();

	mRights.AddTail(new CSeverance);
	mRights.AddTail(new CNotice);
	mpHolidays = new CHolidays;
	mRights.AddTail(mpHolidays);
	mRights.AddTail(new CVacation);
	mRights.AddTail(new CRecuperation);
	mRights.AddTail(new CPension);
	mRights.AddTail(new CAdditional);
	mRights.AddTail(new CPaid);

	/*
	for (int i = 0; i < mn; i++)
	{
		ma[i]->msNameHebrew = hebrew->Get(ma[i]->msName);
	}*/
	return true;
}
void CAllRights::Clear(void)
{
	while (mRights.GetSize() > 0)
	{
		delete mRights.GetTail();
		mRights.RemoveTail();
	}
	mpHolidays = NULL;
	mbComputedOK = false;
	msError = L"";
}
bool CAllRights::SetCheckRef(CButtonRef *pButton)
{
	bool bFound = false;
	POSITION pos = mRights.GetHeadPosition();
	while (pos)
	{
		CRight *pRight = mRights.GetNext(pos);
		if (pRight->SetCheckRef(pButton))
			bFound = true;
	}
	if (!bFound)
	{
		CString s = L"CAllRights::SetCheckRef failed: missing ";
		s += pButton->msName;
		CUtils::MessBox(s, L"SW Error");
		return false;
	}
	return true;
}
bool CAllRights::SetEditRef(CEditRef *pRef)
{
	bool bFound = false;
	POSITION pos = mRights.GetHeadPosition();
	while (pos && !bFound)
	{
		CRight *pRight = mRights.GetNext(pos);
		bFound = pRight->SetEditRef(pRef);
	}
	if (!bFound)
	{
		CString s = L"CAllRights::SetEditRef failed: missing ";
		s += pRef->msName;
		CUtils::MessBox(s, L"SW Error");
		return false;
	}
	return true;
}
bool CAllRights::Compute(void)
{
	static int iCompute = 0;
	static int iComputing = 0;
	iCompute++;

	static bool bInCompute = false;
	if (bInCompute)
		return false;
	bInCompute = true;

	iComputing = iCompute;
	mbComputedOK = ComputeInternal();

	bInCompute = false;
	return mbComputedOK;
}
bool CAllRights::AllInputDefined()
{
	CString sMissing(L"Missing Input:\r\n");
	POSITION pos = mRights.GetHeadPosition();
	while (pos)
	{
		CRight* pRight = mRights.GetNext(pos);
		if (pRight->MissingInput(sMissing))
		{
			if (gpDlg)
				gpDlg->DisplaySummary(sMissing);
			msError = sMissing;
			return false;
		}
	}
	return true;
}
bool CAllRights::ComputeInternal()
{
	if (!AllInputDefined())
		return false;

	bool bOK = true;
	POSITION pos = mRights.GetHeadPosition();
	while (pos)
	{
		CRight *pRight = mRights.GetNext(pos);
		pRight->Init();
	}

	if (!gWorkPeriod.IsValid())
	{
		if (gpDlg)
			gpDlg->DisplaySummary(L"Please define work period");
		msError = L"Work Period not well defined";
		return false;
	}

	if (!gWageTable.Prepare(L"AllRights_Compute"))
	{
		msError = L"Failed to prepare Wage Table!";
		CUtils::MessBox(msError, L"SW Error");
		return false;
	}
	if (!gWageTable.IsValid())
	{
		msError = L"Wage Table not initialized!";
		CUtils::MessBox(msError, L"SW Error");
		return false;
	}

	//  Compute
	pos = mRights.GetHeadPosition();
	while (pos)
	{
		CRight *pRight = mRights.GetNext(pos);
		if (!pRight->ComputeEnvelop())
		{
			msError = pRight->msName;
			msError += " Error: ";
			msError += pRight->msDue;
			bOK = false;
		}
	}

	FILE *pfWrite = CUtils::OpenLogFile(L"Total");
	mSumDue = 0;
	CString sAll(gWorkPeriod.GetTextSummary());
	pos = mRights.GetHeadPosition();
	while (pos)
	{
		CRight *pRight = mRights.GetNext(pos);
		if (pRight->HasLegalValue())
		{
			mSumDue += pRight->mDuePay;
			if (!sAll.IsEmpty())
				sAll += L"\r\n";
			sAll += pRight->msDue;
			CRight::WriteLine(pfWrite, pRight->msDue);
		}
	}

	CRight::WriteLine(pfWrite, L"----------------");

	sAll += L"\r\n==> Total Due ";
	sAll += CRight::ToString(mSumDue);
	if (gpDlg)
		gpDlg->DisplaySummary(sAll);

	CRight::WriteLine(pfWrite, (const wchar_t *)sAll);
	CRight::WriteLine(pfWrite, L"");
	CRight::WriteLine(pfWrite, L"Computations by Worker's Rights Application SW - v0.8, build March, 2019");
	fclose(pfWrite);

	return bOK;
}
void CAllRights::WriteLetterToHtml(CHtmlWriter &html)
{
	for (int iPrint = 0; iPrint < 10; iPrint++)
	{
		POSITION pos = mRights.GetHeadPosition();
		while (pos)
		{
			CRight *pRight = mRights.GetNext(pos);
			if (pRight->miPrintOrder == iPrint && pRight->HasLegalRealValue())
			{
				pRight->WriteLineToHtmlTable(html);
				break;
			}
		}
	}
	WriteTotalLineToHtmlTable(html);
}
void CAllRights::WriteTotalLineToHtmlTable(CHtmlWriter &html)
{
	html.WriteL(L"<tr>");
	
	html.WriteItemToHtmlTable(CString(L"Total Due"), CString(L"סך הכל"));

	CString sEmpty(L"");
	html.WriteItemToHtmlTable(sEmpty, sEmpty);

	html.WriteNumericItemToHtmlTable(mSumDue);

	html.WriteItemToHtmlTable(CString(L"סך הכל"), CString("Total Due"), true);

	html.WriteL(L"</tr>");
}
CHolidays* CAllRights::GetHolidays()
{
	if (!mpHolidays)
		return NULL;
	if (!mpHolidays->InitDefinition())
		return NULL;
	return mpHolidays;
}
CString CAllRights::GetHolidaysSelection()
{
	if (!mpHolidays)
		return CString();
	return mpHolidays->GetSelection();
}
void CAllRights::Save(FILE *pfWrite)
{
	fwprintf(pfWrite, L"<CRights::Save>\n");

	POSITION pos = mRights.GetHeadPosition();
	while (pos)
	{
		CRight *pRight = mRights.GetNext(pos);
		pRight->Save(pfWrite);
	}
}
void CAllRights::SaveToXml(CXMLDump& xmlDump)
{
	CXMLDumpScope scope(L"AllRightsAsComputed", xmlDump);

	POSITION pos = mRights.GetHeadPosition();
	while (pos)
	{
		CRight* pRight = mRights.GetNext(pos);
		pRight->SaveToXml(xmlDump);
	}
}
