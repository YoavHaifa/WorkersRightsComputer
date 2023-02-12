#include "stdafx.h"
#include "HolidaysDue.h"
#include "HolidaysDuePerYear.h"
#include "resource.h"
#include "XMLDump.h"
#include "XMLParse.h"

CHolidaysDue gHolidaysDue;

CHolidaysDue::CHolidaysDue()
	: mSum(IDC_EDIT_HOLIDAYS_PREVY_WORK_SUM,
		IDC_EDIT_HOLIDAYS_PREVY_PAID_SUM, IDC_EDIT_HOLIDAYS_PREVY_FROM_SUM, IDC_EDIT_HOLIDAYS_PREVY_DUE_SUM)
{
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_EDIT_HOLIDAYS_PREVY_WORK,
		IDC_EDIT_HOLIDAYS_PREVY_PAID, IDC_EDIT_HOLIDAYS_PREVY_FROM, IDC_EDIT_HOLIDAYS_PREVY_DUE));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_EDIT_HOLIDAYS_PREVY_WORK1,
		IDC_EDIT_HOLIDAYS_PREVY_PAID1, IDC_EDIT_HOLIDAYS_PREVY_FROM1, IDC_EDIT_HOLIDAYS_PREVY_DUE1));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_EDIT_HOLIDAYS_PREVY_WORK2,
		IDC_EDIT_HOLIDAYS_PREVY_PAID2, IDC_EDIT_HOLIDAYS_PREVY_FROM2, IDC_EDIT_HOLIDAYS_PREVY_DUE2));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_EDIT_HOLIDAYS_PREVY_WORK3,
		IDC_EDIT_HOLIDAYS_PREVY_PAID3, IDC_EDIT_HOLIDAYS_PREVY_FROM3, IDC_EDIT_HOLIDAYS_PREVY_DUE3));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_EDIT_HOLIDAYS_PREVY_WORK4,
		IDC_EDIT_HOLIDAYS_PREVY_PAID4, IDC_EDIT_HOLIDAYS_PREVY_FROM4, IDC_EDIT_HOLIDAYS_PREVY_DUE4));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_EDIT_HOLIDAYS_PREVY_WORK5,
		IDC_EDIT_HOLIDAYS_PREVY_PAID5, IDC_EDIT_HOLIDAYS_PREVY_FROM5, IDC_EDIT_HOLIDAYS_PREVY_DUE5));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_EDIT_HOLIDAYS_PREVY_WORK6,
		IDC_EDIT_HOLIDAYS_PREVY_PAID6, IDC_EDIT_HOLIDAYS_PREVY_FROM6, IDC_EDIT_HOLIDAYS_PREVY_DUE6));
	mHolidaysPerYer.AddTail(new CHolidaysDuePerYear(IDC_EDIT_HOLIDAYS_PREVY_WORK7,
		IDC_EDIT_HOLIDAYS_PREVY_PAID7, IDC_EDIT_HOLIDAYS_PREVY_FROM7, IDC_EDIT_HOLIDAYS_PREVY_DUE7));
}
void CHolidaysDue::InitDialog(CPrevYearsHolidaysDlg* pDlg)
{
	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		pYear->UpdateGui(pDlg);
	}
	UpdateSum(pDlg);
}
void CHolidaysDue::OnGuiChange(int iYear, CPrevYearsHolidaysDlg* pDlg)
{
	static bool umbHandlingChange = false;
	if (umbHandlingChange)
		return;
	umbHandlingChange = true;

	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		if (pYear->mId == iYear)
		{
			pYear->OnGuiChange(pDlg);
			break;
		}
	}
	UpdateSum(pDlg);
	umbHandlingChange = false;
}
void CHolidaysDue::UpdateSum(CPrevYearsHolidaysDlg* pDlg)
{
	mSum.Zero();
	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		mSum.Add(*pYear);
	}
	mSum.UpdateGui(pDlg);
}
void CHolidaysDue::SaveToXml(CXMLDump& xmlDump)
{
	CXMLDumpScope scope(L"HolidaysDue", xmlDump);

	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		pYear->SaveToXml(xmlDump);
	}
}
void CHolidaysDue::LoadFromXml(class CXMLParseNode* pRoot)
{
	CXMLParseNode* pMain = pRoot->GetFirst(L"HolidaysDue");
	if (!pMain)
		return;

	CXMLParseNode* pYearNode = pMain->GetFirst(L"Year");
	POSITION pos = mHolidaysPerYer.GetHeadPosition();
	while (pYearNode && pos)
	{
		CHolidaysDuePerYear* pYear = mHolidaysPerYer.GetNext(pos);
		pYear->LoadFromXml(pYearNode);

		pYearNode = pMain->GetNext(L"Year", pYearNode);
	}
}

