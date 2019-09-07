// VacationsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "VacationsDlg.h"
#include "VacationUsed.h"
#include "UsedVacations.h"
#include "Utils.h"
#include "WorkPeriod.h"
#include "afxdialogex.h"


// CVacationsDlg dialog

IMPLEMENT_DYNAMIC(CVacationsDlg, CDialogEx)

CVacationsDlg::CVacationsDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_VACATIONS, pParent)
{

}

CVacationsDlg::~CVacationsDlg()
{
}

void CVacationsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER_VAC_START, mEditFirstDay);
	DDX_Control(pDX, IDC_DATETIMEPICKER_VAC_END, mEditLastDay);
}


BEGIN_MESSAGE_MAP(CVacationsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_VAC_ADD, &CVacationsDlg::OnBnClickedButtonVacAdd)
	ON_BN_CLICKED(IDC_BUTTON_VAC_CLEAR_LAST, &CVacationsDlg::OnBnClickedButtonVacClearLast)
	ON_BN_CLICKED(IDC_BUTTON_VAC_CLEAR, &CVacationsDlg::OnBnClickedButtonVacClear)
	ON_BN_CLICKED(IDC_CHECK_14DAYS_VAC4SEVRANCE, &CVacationsDlg::OnBnClickedCheck14daysVac4sevrance)
END_MESSAGE_MAP()

BOOL CVacationsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (gUsedVacations.mbAdd14DaysUnpaidVacation4Severance)
		SetCheck(IDC_CHECK_14DAYS_VAC4SEVRANCE, true);

	UpdateText();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// CVacationsDlg message handlers


void CVacationsDlg::OnBnClickedButtonVacAdd()
{
	CTime time;
	DWORD flags = mEditFirstDay.GetTime(time);
	mStart.SetDate(time);
	flags = mEditLastDay.GetTime(time);
	mEnd.SetDate(time);

	if (mStart > mEnd)
	{
		CUtils::MessBox(L"Last day is before first day", L"Input Error");
		return;
	}

	if (mStart <= gWorkPeriod.mFirst)
	{
		CUtils::MessBox(L"Vacation starts before first work day", L"Input Error");
		return;
	}
	if (mEnd > gWorkPeriod.mLast)
	{
		CUtils::MessBox(L"Vacation ends after last work day", L"Input Error");
		return;
	}

	CVacationUsed *pVac = new CVacationUsed(mStart, mEnd);
	gUsedVacations.AddVacation(pVac);
	UpdateText();
}
void CVacationsDlg::UpdateText()
{
	CString s = gUsedVacations.GetVacationsFullText();
	SetText(IDC_EDIT_VAC_LIST, s);
}
void CVacationsDlg::OnBnClickedButtonVacClearLast()
{
	gUsedVacations.ClearLastVacation();
	UpdateText();
}
void CVacationsDlg::OnBnClickedButtonVacClear()
{
	gUsedVacations.ClearAllVacations();
	UpdateText();
}
void CVacationsDlg::OnBnClickedCheck14daysVac4sevrance()
{
	gUsedVacations.mbAdd14DaysUnpaidVacation4Severance = IsChecked(IDC_CHECK_14DAYS_VAC4SEVRANCE);
	UpdateText();
}
