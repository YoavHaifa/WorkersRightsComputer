// WagePeriods.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "WagePeriodsDlg.h"
#include "WorkPeriod.h"
#include "Wage.h"
#include "Utils.h"
#include "afxdialogex.h"

CWagePeriodsDlg* CWagePeriodsDlg::umpDlg = NULL;

// CWagePeriods dialog

IMPLEMENT_DYNAMIC(CWagePeriodsDlg, CDialogEx)

CWagePeriodsDlg::CWagePeriodsDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_WAGE_PERIODS, pParent)
{

}
CWagePeriodsDlg::~CWagePeriodsDlg()
{
	umpDlg = NULL;
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

	if (gWage.IsSinglePeriod())
	{
		mStartPeriod.SetTime(&gWorkPeriod.mFirst.mTime);
		mLastInPeriod.SetTime(&gWorkPeriod.mLast.mTime);
	}
	UpdateState();
	umpDlg = this;
	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CWagePeriodsDlg::UpdateState(const char* zTitle)
{
	// CString sState("Wage Periods\r\n");
	msState = "";
	if (zTitle)
	{
		msState += zTitle;
		msState += "\r\n";
	}
	msState += gWage.GetStateDesc();

	mShowPeriods.SetWindowText(msState);
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
		GetParameter(IDC_EDIT_MONTH_SALARY, mMonthly, 0, 100000);
		if (mMonthly == 0)
		{
			CUtils::MessBox(L"Please define monthly wage", L"Input Error");
			return false;
		}
	}
	else if (IsChecked(IDC_RADIO_HOURLY))
	{
		meMode = WAGE_HOURLY;
		GetParameter(IDC_EDIT_HOUR_SALARY, mHourly, 0, 10000);
		if (mHourly == 0)
		{
			CUtils::MessBox(L"Please define hourly wage", L"Input Error");
			return false;
		}
		GetParameter(IDC_EDIT_HOURS_PER_MONTH, mnHoursPerMonth, 0, 1000);
		if (mnHoursPerMonth == 0)
		{
			CUtils::MessBox(L"Please define number of hours per month", L"Input Error");
			return false;
		}
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

	CWagePeriod* pPeriod = NULL;
	switch (meMode)
	{
	case WAGE_MIN:
		pPeriod = new CWagePeriod(meMode, mFirst, mLast);
		break;
	case WAGE_MONTHLY:
		pPeriod = new CWagePeriod(meMode, mFirst, mLast, mMonthly);
		break;
	case WAGE_HOURLY:
		pPeriod = new CWagePeriod(meMode, mFirst, mLast, mHourly, mnHoursPerMonth);
		break;
	}

	gWage.AddPeriod(pPeriod);
	UpdateState();
}
