// RealHolidaysDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "RealHolidaysDlg.h"
#include "afxdialogex.h"
#include "Utils.h"
#include "FilesList.h"
#include "WorkPeriod.h"
#include "HolidaysByDay.h"


// CRealHolidaysDlg dialog

IMPLEMENT_DYNAMIC(CRealHolidaysDlg, CDialogEx)

CRealHolidaysDlg::CRealHolidaysDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_REAL_HOLIDAYS, pParent)
{
	mapCheckDays[0] = &mCheckSunday;
	mapCheckDays[1] = &mCheckMonday;
	mapCheckDays[2] = &mCheckTuesday;
	mapCheckDays[3] = &mCheckWednesday;
	mapCheckDays[4] = &mCheckThursday;
	mapCheckDays[5] = &mCheckFriday;
	mapCheckDays[6] = &mCheckSaturday;

}

CRealHolidaysDlg::~CRealHolidaysDlg()
{
}

void CRealHolidaysDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_HOLIDAYS, mComboHolidays);

	DDX_Control(pDX, IDC_CHECK_SUNDAY, mCheckSunday);
	DDX_Control(pDX, IDC_CHECK_MONDAY, mCheckMonday);
	DDX_Control(pDX, IDC_CHECK_TUESDAY, mCheckTuesday);
	DDX_Control(pDX, IDC_CHECK_WEDNESDAY, mCheckWednesday);
	DDX_Control(pDX, IDC_CHECK_THURSDAY, mCheckThursday);
	DDX_Control(pDX, IDC_CHECK_FRIDAY, mCheckFriday);
	DDX_Control(pDX, IDC_CHECK_SATURDAY, mCheckSaturday);
}

BOOL CRealHolidaysDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitHolidaysCombo();
	mComboHolidays.SetWindowTextW(gHolidaysByDay.msSelectedHolidays);

	for (int iDay = 0; iDay < N_WEEK_DAYS; iDay++)
	{
		if (gWorkPeriod.IsWorkingDay(iDay))
			mapCheckDays[iDay]->SetCheck(BST_CHECKED);
		else
			mapCheckDays[iDay]->SetCheck(BST_UNCHECKED);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CRealHolidaysDlg::OnOK()
{
	OnCbnSelchangeComboHolidays();
	gWorkPeriod.Compute();
	CDialogEx::OnOK();
}


BEGIN_MESSAGE_MAP(CRealHolidaysDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_HOLIDAYS, &CRealHolidaysDlg::OnCbnSelchangeComboHolidays)
	ON_BN_CLICKED(IDC_CHECK_SUNDAY, &CRealHolidaysDlg::OnBnClickedCheckSunday)
	ON_BN_CLICKED(IDC_CHECK_MONDAY, &CRealHolidaysDlg::OnBnClickedCheckMonday)
	ON_BN_CLICKED(IDC_CHECK_TUESDAY, &CRealHolidaysDlg::OnBnClickedCheckTuesday)
	ON_BN_CLICKED(IDC_CHECK_WEDNESDAY, &CRealHolidaysDlg::OnBnClickedCheckWednesday)
	ON_BN_CLICKED(IDC_CHECK_THURSDAY, &CRealHolidaysDlg::OnBnClickedCheckThursday)
	ON_BN_CLICKED(IDC_CHECK_FRIDAY, &CRealHolidaysDlg::OnBnClickedCheckFriday)
	ON_BN_CLICKED(IDC_CHECK_SATURDAY, &CRealHolidaysDlg::OnBnClickedCheckSaturday)
END_MESSAGE_MAP()


// CRealHolidaysDlg message handlers
void CRealHolidaysDlg::InitHolidaysCombo()
{
	CString msDir(CUtils::GetBaseDir() + L"input\\holidays");
	CFilesList list;
	CUtils::ListFilesInDir(msDir, L"txt", list);
	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CString* psName = list.GetNext(pos);
		CFileName fName(*psName);
		CString sPrivate = fName.PrivateWithoutType();
		if (sPrivate.Right(8) == "Holidays")
			sPrivate = sPrivate.Left(sPrivate.GetLength() - 8);
		mComboHolidays.AddString(sPrivate);
	}
}
void CRealHolidaysDlg::OnCbnSelchangeComboHolidays()
{
	CString s;
	mComboHolidays.GetWindowTextW(s);
	gHolidaysByDay.SetSelectionByUser(s);
}
bool CRealHolidaysDlg::UpdateDaysDataFromDialog()
{
	for (int iDay = 0; iDay < N_WEEK_DAYS; iDay++)
	{
		if (mapCheckDays[iDay]->GetCheck() == BST_CHECKED)
			gWorkPeriod.SetWorkingDayByUser(iDay, 1);
		else
			gWorkPeriod.SetWorkingDayByUser(iDay, 0);
	}
	return true;
}
void CRealHolidaysDlg::OnBnClickedCheckSunday()
{
	UpdateDaysDataFromDialog();
}
void CRealHolidaysDlg::OnBnClickedCheckMonday()
{
	UpdateDaysDataFromDialog();
}
void CRealHolidaysDlg::OnBnClickedCheckTuesday()
{
	UpdateDaysDataFromDialog();
}
void CRealHolidaysDlg::OnBnClickedCheckWednesday()
{
	UpdateDaysDataFromDialog();
}
void CRealHolidaysDlg::OnBnClickedCheckThursday()
{
	UpdateDaysDataFromDialog();
}
void CRealHolidaysDlg::OnBnClickedCheckFriday()
{
	UpdateDaysDataFromDialog();
}
void CRealHolidaysDlg::OnBnClickedCheckSaturday()
{
	UpdateDaysDataFromDialog();
}
