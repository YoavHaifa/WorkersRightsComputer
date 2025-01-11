// RealHolidaysDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "RealHolidaysDlg.h"
#include "afxdialogex.h"
#include "Utils.h"
#include "FilesList.h"
#include "WorkPeriod.h"


// CRealHolidaysDlg dialog

IMPLEMENT_DYNAMIC(CRealHolidaysDlg, CDialogEx)

CRealHolidaysDlg::CRealHolidaysDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_REAL_HOLIDAYS, pParent)
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

	for (int iDay = 0; iDay < N_WEEK_DAYS; iDay++)
	{
		if (gWorkPeriod.maWorkingDays[iDay] > 0)
			mapCheckDays[iDay]->SetCheck(BST_CHECKED);
		else
			mapCheckDays[iDay]->SetCheck(BST_UNCHECKED);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CRealHolidaysDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_HOLIDAYS, &CRealHolidaysDlg::OnCbnSelchangeComboHolidays)
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
	//CString s1 = GetText(IDC_COMBO_HOLIDAYS);
	//OnInputChange();
}
bool CRealHolidaysDlg::UpdateDataFromDialog(void)
{
	for (int iDay = 0; iDay < N_WEEK_DAYS; iDay++)
	{
		if (mapCheckDays[iDay]->GetCheck() == BST_CHECKED)
			gWorkPeriod.SetWorkingDay(iDay, 1);
		else
			gWorkPeriod.SetWorkingDay(iDay, 0);
	}
	return true;
}
CString CRealHolidaysDlg::GetDaysText()
{
	int nDays = 0;
	CString sDays(_T("("));
	for (int iDay = 0; iDay < N_WEEK_DAYS; iDay++)
	{
		if (mapCheckDays[iDay]->GetCheck() == BST_CHECKED)
		{
			nDays++;
			if (nDays > 1)
				sDays += L", ";
			switch (iDay)
			{
			case 0:
				sDays += L"Sun";
				break;
			case 1:
				sDays += L"Mon";
				break;
			case 2:
				sDays += L"Tue";
				break;
			case 3:
				sDays += L"Wed";
				break;
			case 4:
				sDays += L"Thu";
				break;
			case 5:
				sDays += L"Fri";
				break;
			case 6:
				sDays += L"Sat";
				break;
			}
		}
	}
	sDays += L")";
	if (nDays < 1)
		return CString();

	wchar_t zBuf[512];
	swprintf_s(zBuf, sizeof(zBuf) / sizeof(wchar_t), L"%d working days per week ", nDays);
	CString s(zBuf);
	s += sDays;
	s += L"\r\n";
	return s;
}
