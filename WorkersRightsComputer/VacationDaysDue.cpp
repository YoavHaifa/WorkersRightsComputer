#include "stdafx.h"
#include "VacationDaysDue.h"
#include "VacationDuePerYear.h"
#include "resource.h"
#include "MyDialogEx.h"
#include "WorkYear.h"
#include "WorkYears.h"

CVacationDaysDue gVacationDaysDue;

CVacationDaysDue::CVacationDaysDue()
	: mbAskForFullYears(false)
	, mnFullYears(0)
	, mbAskForPartialYears(false)
	, mnDaysDue(0)
	, mn(0)
{
	mVacationPerYears.AddTail(new CVacationDuePerYear(IDC_STATIC_VAC_PY, IDC_STATIC_VACPY_PAID,
		IDC_RADIO_ALL_LAST_YEAR_VACATION, IDC_RADIO_LAST_YEAR_VACATION_DAYS,
		IDC_EDIT_LAST_YEAR_VACATION_DAYS, IDC_STATIC_VACATION_ALL,
		IDC_RADIO_LAST_YEAR_VACATION_NONE, IDC_STATIC_VACPY_DUE, IDC_EDIT_VACATION_PY_DUE));

	mVacationPerYears.AddTail(new CVacationDuePerYear(IDC_STATIC_VAC_PY2, IDC_STATIC_VACPY_PAID2,
		IDC_RADIO_ALL_LAST_YEAR_VACATION2, IDC_RADIO_LAST_YEAR_VACATION_DAYS2,
		IDC_EDIT_LAST_YEAR_VACATION_DAYS2, IDC_STATIC_VACATION_ALL2,
		IDC_RADIO_LAST_YEAR_VACATION_NONE2, IDC_STATIC_VACPY_DUE2, IDC_EDIT_VACATION_PY_DUE2));

	mVacationPerYears.AddTail(new CVacationDuePerYear(IDC_STATIC_VAC_PY3, IDC_STATIC_VACPY_PAID3,
		IDC_RADIO_ALL_LAST_YEAR_VACATION3, IDC_RADIO_LAST_YEAR_VACATION_DAYS3,
		IDC_EDIT_LAST_YEAR_VACATION_DAYS3, IDC_STATIC_VACATION_ALL3,
		IDC_RADIO_LAST_YEAR_VACATION_NONE3, IDC_STATIC_VACPY_DUE3, IDC_EDIT_VACATION_PY_DUE3));

	mVacationPerYears.AddTail(new CVacationDuePerYear(IDC_STATIC_VAC_PY4, IDC_STATIC_VACPY_PAID4,
		IDC_RADIO_ALL_LAST_YEAR_VACATION4, IDC_RADIO_LAST_YEAR_VACATION_DAYS4,
		IDC_EDIT_LAST_YEAR_VACATION_DAYS4, IDC_STATIC_VACATION_ALL4,
		IDC_RADIO_LAST_YEAR_VACATION_NONE4, IDC_STATIC_VACPY_DUE4, IDC_EDIT_VACATION_PY_DUE4));

	mpThisYearVacationDays = mVacationPerYears.GetHead();

}
void CVacationDaysDue::LoadFromXml(CXMLParseNode* /*pRoot*/)
{
	SetYearsByWorkPeriod();
}
bool CVacationDaysDue::VerifyWorkPeriod(class CMyDialogEx* /*pMainDlg*/)
{
	SetYearsByWorkPeriod();
	return true;
}
void CVacationDaysDue::Reset()
{
	POSITION pos = mVacationPerYears.GetHeadPosition();
	while (pos)
	{
		CVacationDuePerYear* pYear = mVacationPerYears.GetNext(pos);
		pYear->Zero();
	}
}
void CVacationDaysDue::SetYearsByWorkPeriod()
{
	mn = 0;
	int iFromLast = 0;
	POSITION pos = mVacationPerYears.GetHeadPosition();
	while (pos)
	{
		CVacationDuePerYear* pYear = mVacationPerYears.GetNext(pos);
		CWorkYear* pWorkYear = gWorkYears.GetByReverseIndex(iFromLast);
		if (pWorkYear)
		{
			pYear->Init(pWorkYear);
			mn++;
		}
		else
			pYear->SetIgnore();
		iFromLast++;
	}
}
void CVacationDaysDue::InitDialog(CMyDialogEx* pDlg)
{
	POSITION pos = mVacationPerYears.GetHeadPosition();
	while (pos)
	{
		CVacationDuePerYear* pYear = mVacationPerYears.GetNext(pos);
		pYear->UpdateGui(pDlg);
	}
	//UpdateSum(pDlg);
	//mbDefinedBySpecialDialog = true;
}