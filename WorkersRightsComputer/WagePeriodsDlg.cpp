// WagePeriods.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "WagePeriodsDlg.h"
#include "Wage.h"
#include "Utils.h"
#include "afxdialogex.h"


// CWagePeriods dialog

IMPLEMENT_DYNAMIC(CWagePeriodsDlg, CDialogEx)

CWagePeriodsDlg::CWagePeriodsDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_WAGE_PERIODS, pParent)
{

}
CWagePeriodsDlg::~CWagePeriodsDlg()
{
}
void CWagePeriodsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PERIODS, mShowPeriods);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, mStartPeriod);
	DDX_Control(pDX, IDC_DATETIMEPICKER_LAST, mLastInPeriod);
}

BEGIN_MESSAGE_MAP(CWagePeriodsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD_WAGE_PERIOD, &CWagePeriodsDlg::OnBnClickedButtonAddWagePeriod)
END_MESSAGE_MAP()

// CWagePeriods message handlers

BOOL CWagePeriodsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetCheck(IDC_CHECK_WAGE_PERIODS_FULL_MONTHS, true);

	UpdateState();
	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CWagePeriodsDlg::UpdateState()
{
	// CString sState("Wage Periods\r\n");
	CString sState = gWage.GetStateText();

	mShowPeriods.SetWindowText(sState);
}
bool CWagePeriodsDlg::GetInputTime()
{
	CTime time;
	DWORD flags = mStartPeriod.GetTime(time);
	mFirst.SetDate(time);
	flags = mLastInPeriod.GetTime(time);
	mLast.SetDate(time);

	if (!mFirst.mbInitialized || !mLast.mbInitialized)
	{
		CUtils::MessBox(L"Please define start and last dates of period", L"Input Error");
		return false;
	}

	if (mFirst > mLast)
	{
		CUtils::MessBox(L"End of period before its start", L"Input Error");
		return false;
	}
	return true;
}
bool CWagePeriodsDlg::GetWage()
{
	if (IsChecked(IDC_RADIO_MIN_WAGE))
	{
		meMode = WAGE_MIN;
	}
	else if (IsChecked(IDC_RADIO_MONTHLY))
	{
		meMode = WAGE_MONTHLY;
	}
	else if (IsChecked(IDC_RADIO_HOURLY))
	{
		meMode = WAGE_HOURLY;
	}
	else
	{
		CUtils::MessBox(L"Please define wage mode (min, monthly or hourly)", L"Input Error");
		return false;
	}
	return true;
}
void CWagePeriodsDlg::OnBnClickedButtonAddWagePeriod()
{
	if (!GetInputTime())
		return;

	if (!GetWage())
		return;




	CUtils::MessBox(L"Period Added", L"Notice");
}
