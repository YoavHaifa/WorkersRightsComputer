// CWorkPeriodDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "WorkPeriodDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "WorkPeriod.h"
#include "Right.h"
#include "VacationsDlg.h"
#include "UsedVacations.h"
#include "FamilyPartDlg.h"
#include "FamilyPart.h"
#include "Utils.h"
#include "Config.h"
#include "WagePeriodsDlg.h"
#include "Wage.h"

IMPLEMENT_DYNAMIC(CWorkPeriodDlg, CDialogEx)

CWorkPeriodDlg::CWorkPeriodDlg(CWnd* pParent /*=nullptr*/)
	: CWageDefBaseDlg(IDD_DIALOG_WORK_PERIOD, pParent)
{
	mapCheckDays[0] = &mCheckSunday;
	mapCheckDays[1] = &mCheckMonday;
	mapCheckDays[2] = &mCheckTuesday;
	mapCheckDays[3] = &mCheckWednesday;
	mapCheckDays[4] = &mCheckThursday;
	mapCheckDays[5] = &mCheckFriday;
	mapCheckDays[6] = &mCheckSaturday;
}
CWorkPeriodDlg::~CWorkPeriodDlg()
{
}
void CWorkPeriodDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, mStartDate);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, mEndDate);
	DDX_Control(pDX, IDC_DATETIMEPICKER_NOTICE, mNoticeDate);
	DDX_Control(pDX, IDC_EDIT_PERIODS, mShow);
	DDX_Control(pDX, IDC_EDIT_MONTH_SALARY, mMonthlySalary);
	DDX_Control(pDX, IDC_EDIT_HOUR_SALARY, mHourlySalary);
	DDX_Control(pDX, IDC_EDIT_HOURS_PER_MONTH, mHoursPerWeek);
	DDX_Control(pDX, IDC_CHECK_SUNDAY, mCheckSunday);
	DDX_Control(pDX, IDC_CHECK_MONDAY, mCheckMonday);
	DDX_Control(pDX, IDC_CHECK_TUESDAY, mCheckTuesday);
	DDX_Control(pDX, IDC_CHECK_WEDNESDAY, mCheckWednesday);
	DDX_Control(pDX, IDC_CHECK_THURSDAY, mCheckThursday);
	DDX_Control(pDX, IDC_CHECK_FRIDAY, mCheckFriday);
	DDX_Control(pDX, IDC_CHECK_SATURDAY, mCheckSaturday);

	DDX_Control(pDX, IDC_RADIO_MIN_WAGE, mRadioMinWage);
	DDX_Control(pDX, IDC_RADIO_MONTHLY, mRadioMonthly);
	DDX_Control(pDX, IDC_RADIO_HOURLY, mRadioHourly);
	DDX_Control(pDX, IDC_RADIO_DIFF_WAGES, mRadioWagePeriods);
	//DDX_Control(pDX, IDC_DATETIMEPICKER_NOTICE2, mLastSalaryDate);
}


BEGIN_MESSAGE_MAP(CWorkPeriodDlg, CDialogEx)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_START, &CWorkPeriodDlg::OnDtnDatetimechangeDatetimepickerStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_END, &CWorkPeriodDlg::OnDtnDatetimechangeDatetimepickerEnd)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_NOTICE, &CWorkPeriodDlg::OnDtnDatetimechangeDatetimepickerNotice)
	ON_BN_CLICKED(IDC_RADIO_MIN_WAGE, &CWorkPeriodDlg::OnBnClickedRadioMinWage)
	ON_BN_CLICKED(IDC_RADIO_MONTHLY, &CWorkPeriodDlg::OnBnClickedRadioMonthly)
	ON_BN_CLICKED(IDC_RADIO_HOURLY, &CWorkPeriodDlg::OnBnClickedRadioHourly)
	ON_EN_CHANGE(IDC_EDIT_MONTH_SALARY, &CWorkPeriodDlg::OnEnChangeEditMonthSalary)
	ON_EN_CHANGE(IDC_EDIT_HOUR_SALARY, &CWorkPeriodDlg::OnEnChangeEditHourSalary)
	ON_EN_CHANGE(IDC_EDIT_HOURS_PER_MONTH, &CWorkPeriodDlg::OnEnChangeEditHoursPerMonth)
	ON_BN_CLICKED(IDOK, &CWorkPeriodDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_SUNDAY, &CWorkPeriodDlg::OnBnClickedCheckSunday)
	ON_BN_CLICKED(IDC_CHECK_MONDAY, &CWorkPeriodDlg::OnBnClickedCheckMonday)
	ON_BN_CLICKED(IDC_CHECK_TUESDAY, &CWorkPeriodDlg::OnBnClickedCheckTuesday)
	ON_BN_CLICKED(IDC_CHECK_WEDNESDAY, &CWorkPeriodDlg::OnBnClickedCheckWednesday)
	ON_BN_CLICKED(IDC_CHECK_THURSDAY, &CWorkPeriodDlg::OnBnClickedCheckThursday)
	ON_BN_CLICKED(IDC_CHECK_FRIDAY, &CWorkPeriodDlg::OnBnClickedCheckFriday)
	ON_BN_CLICKED(IDC_CHECK_SATURDAY, &CWorkPeriodDlg::OnBnClickedCheckSaturday)
	ON_BN_CLICKED(IDOK3, &CWorkPeriodDlg::OnBnClickedOk3)
	ON_BN_CLICKED(IDC_BUTTON_FAMILY_PART, &CWorkPeriodDlg::OnBnClickedButtonFamilyPart)
	ON_BN_CLICKED(IDC_CHECK_NO_NOTICE, &CWorkPeriodDlg::OnBnClickedCheckNoNotice)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_WAGE, &CWorkPeriodDlg::OnBnClickedButtonEditWage)
	ON_BN_CLICKED(IDC_RADIO_DIFF_WAGES, &CWorkPeriodDlg::OnBnClickedRadioDiffWages)
	ON_BN_CLICKED(IDC_BUTTON_SET_WAGE, &CWorkPeriodDlg::OnBnClickedButtonSetWage)
	ON_BN_CLICKED(IDC_CHECK_MONTHLY_BONUS, &CWorkPeriodDlg::OnBnClickedCheckMonthlyBonus)
END_MESSAGE_MAP()

void CWorkPeriodDlg::SetWageGui()
{
	mRadioMinWage.SetCheck(BST_UNCHECKED);
	mRadioMonthly.SetCheck(BST_UNCHECKED);
	mRadioHourly.SetCheck(BST_UNCHECKED);
	mRadioWagePeriods.SetCheck(BST_UNCHECKED);

	EWageMode eWageMode;
	if (gWage.IsSinglePeriod(eWageMode))
	{
		switch (eWageMode)
		{
		case WAGE_MIN:
			mRadioMinWage.SetCheck(BST_CHECKED);
			break;
		case WAGE_MONTHLY:
			mRadioMonthly.SetCheck(BST_CHECKED);
			SetWageMode(IDC_RADIO_MONTHLY);
			mMonthlySalary.SetWindowTextW(CRight::ToString(gWage.GetMonthlyWage()));
			break;
		case WAGE_HOURLY:
			mRadioHourly.SetCheck(BST_CHECKED);
			SetWageMode(IDC_RADIO_HOURLY);
			mHourlySalary.SetWindowTextW(CRight::ToString(gWage.GetHourlyWage()));
			mHoursPerWeek.SetWindowTextW(CRight::ToString(gWage.GetHoursPerMonth()));
		}
		Enable(IDC_BUTTON_SET_WAGE);
	}
	else // Different wage periods as defined by the user in inner dialog
	{
		mRadioWagePeriods.SetCheck(BST_CHECKED);
		SetWageMode(WAGE_UNDEF);
		Disable(IDC_BUTTON_SET_WAGE);
	}
}
BOOL CWorkPeriodDlg::OnInitDialog()
{
	CWageDefBaseDlg::OnInitDialog();

	mMonthlySalary.EnableWindow(FALSE);
	mHourlySalary.EnableWindow(FALSE);
	mHoursPerWeek.EnableWindow(FALSE);

	if (gWorkPeriod.mFirst.mbInitialized)
	{
		mStartDate.SetTime(&gWorkPeriod.mFirst.mTime);
		mbStartSet = true;
	}
	else
	{
		SetInvisible(IDC_DATETIMEPICKER_END);
		SetInvisible(IDC_DATETIMEPICKER_NOTICE);
	}

	if (gWorkPeriod.mLast.mbInitialized)
	{
		mbEndSet = true;
		mEndDate.SetTime(&gWorkPeriod.mLast.mTime);
		SetVisible(IDC_DATETIMEPICKER_END);
	}
	else
	{
		SetInvisible(IDC_DATETIMEPICKER_NOTICE);
	}

	if (gWorkPeriod.mNotice.mbInitialized)
	{
		mbNoticeSet = true;
		mNoticeDate.SetTime(&gWorkPeriod.mNotice.mTime);
		SetVisible(IDC_DATETIMEPICKER_NOTICE);
	}
	SetCheck(IDC_CHECK_NO_NOTICE, gWorkPeriod.mbSkipNotice);

	for (int iDay = 0; iDay < 7; iDay++)
	{
		if (gWorkPeriod.maWorkingDays[iDay] > 0)
			mapCheckDays[iDay]->SetCheck(BST_CHECKED);
		else
			mapCheckDays[iDay]->SetCheck(BST_UNCHECKED);
	}

	SetWageGui();

	UpdateText();
	return TRUE;  // return TRUE  unless you set the focus to a control
}
// CWorkPeriodDlg message handlers

void CWorkPeriodDlg::OnDtnDatetimechangeDatetimepickerStart(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	//LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	mbStartSet = true;
	UpdateText();
	*pResult = 0;
	SetVisible(IDC_DATETIMEPICKER_END);
}

void CWorkPeriodDlg::OnDtnDatetimechangeDatetimepickerEnd(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	// LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	mbEndSet = true;
	UpdateText();
	*pResult = 0;
	SetVisible(IDC_DATETIMEPICKER_NOTICE);
	if (!mbNoticeSet)
	{
		CTime time;
		mEndDate.GetTime(time);
		mNoticeDate.SetTime(&time);
	}
}

void CWorkPeriodDlg::OnDtnDatetimechangeDatetimepickerNotice(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	// LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	mbNoticeSet = true;
	UpdateText();
	*pResult = 0;
}
int CWorkPeriodDlg::UpdateText()
{
	CString str;
	CString sAll;

	CTime timeTime;
	if (mbStartSet)
	{
		DWORD dwResult = mStartDate.GetTime(timeTime);
		if (dwResult == GDT_VALID)
		{
			str = timeTime.Format(_T("%#x"));
			sAll = "Started working at: ";
			sAll += str;
			sAll += "\r\n";
		}
	}
	if (mbEndSet)
	{
		DWORD dwResult = mEndDate.GetTime(timeTime);
		if (dwResult == GDT_VALID)
		{
			str = timeTime.Format(_T("%#x"));
			sAll += "Finished working at: ";
			sAll += str;
			sAll += "\r\n";
		}
	}
	if (gWorkPeriod.mbSkipNotice)
	{
		sAll += "Do not demand notice.\r\n";
	}
	else if (mbNoticeSet)
	{
		DWORD dwResult = mNoticeDate.GetTime(timeTime);
		if (dwResult == GDT_VALID)
		{
			str = timeTime.Format(_T("%#x"));
			sAll += "End of work notice received at: ";
			sAll += str;
			sAll += "\r\n";
		}
	}

	sAll += GetDaysText();

	sAll += gUsedVacations.GetVacationsShortText();
	sAll += "\r\n";

	sAll += gWage.GetShortText();
	sAll += "\r\n";

	if (gFamilyPart.mbAskOnlyForFamilyPart)
	{
		sAll += gFamilyPart.GetShortText();
		sAll += "\r\n";
	}

	mShow.SetWindowText(sAll);
	//CWnd wnd = GetDialogEl
	return 0;
}
void CWorkPeriodDlg::OnBnClickedRadioMinWage()
{
	SetWageMode(IDC_RADIO_MIN_WAGE);
}
void CWorkPeriodDlg::OnBnClickedRadioMonthly()
{
	SetWageMode(IDC_RADIO_MONTHLY);
}
void CWorkPeriodDlg::OnBnClickedRadioHourly()
{
	SetWageMode(IDC_RADIO_HOURLY);
}
void CWorkPeriodDlg::SetWageMode(int mode)
{
	CString sEmpty;
	mMonthlySalary.SetWindowText(sEmpty);
	mHourlySalary.SetWindowText(sEmpty);
	mHoursPerWeek.SetWindowText(sEmpty);

	mMonthlySalary.EnableWindow(mode == IDC_RADIO_MONTHLY);
	mHourlySalary.EnableWindow(mode == IDC_RADIO_HOURLY);
	mHoursPerWeek.EnableWindow(mode == IDC_RADIO_HOURLY);
}
/*
CString CWorkPeriodDlg::GetWageText()
{
	if (mWageMode == IDC_RADIO_MIN_WAGE)
		return CString(L"Wage: Minimum Wage");
	if (mWageMode == IDC_RADIO_MONTHLY)
	{
		CString sText;
		mMonthlySalary.GetWindowText(sText);
		if (sText.IsEmpty())
			return CString(L"Wage: Monthly - insert bruto");
		else
		{
			CString s(L"Wage: Monthly - ");
			s += sText;
			s += L" shekel bruto";
			return s;

		}
	}
	if (mWageMode == IDC_RADIO_HOURLY)
	{
		CString sRet;
		CString sText;
		mHourlySalary.GetWindowText(sText);
		if (sText.IsEmpty())
			return CString(L"Wage: Hourly - insert rate per hour");
		else
		{
			sRet = L"Wage: Hourly - ";
			sRet += sText;
			sRet += L" shekel per hour";
		}

		sRet += L"\r\n";
		mHoursPerWeek.GetWindowText(sText);
		if (sText.IsEmpty())
		{
			sRet += L"Insert number of hours per week.";
		}
		else
		{
			sRet += sText;
			sRet += L" hours per week.";
		}
		sRet += L"\r\n";
		return sRet;
	}
	return CString(L"Wage: Not Defined");
}*/
void CWorkPeriodDlg::OnEnChangeEditMonthSalary()
{
	UpdateText();
}
void CWorkPeriodDlg::OnEnChangeEditHourSalary()
{
	UpdateText();
}
void CWorkPeriodDlg::OnEnChangeEditHoursPerMonth()
{
	UpdateText();
}
bool CWorkPeriodDlg::UpdateDataFromDialog(void)
{
	CTime time;
	DWORD flags = mStartDate.GetTime(time);
	gWorkPeriod.mFirst.SetDate(time);

	flags = mEndDate.GetTime(time);
	gWorkPeriod.mLast.SetDate(time);
	flags = mNoticeDate.GetTime(time);
	gWorkPeriod.mNotice.SetDate(time);

	for (int iDay = 0; iDay < 7; iDay++)
	{
		if (mapCheckDays[iDay]->GetCheck() == BST_CHECKED)
			gWorkPeriod.SetWorkingDay(iDay, 1);
		else
			gWorkPeriod.SetWorkingDay(iDay, 0);
	}

	bool bOK = SetWageForWholePeriod();
	bOK &= gWage.VerifyWorkPeriod();
	return bOK;

	//if (IsChecked(IDC_CHECK_NOT_INCLUDING))
	//	gWorkPeriod.mbNotIncludingLastSalary = true;
	//else
	//	gWorkPeriod.mbNotIncludingLastSalary = false;

	//flags = mLastSalaryDate.GetTime(time);
	//gWorkPeriod.mLastSalaryUntil.SetDate(time);
}
void CWorkPeriodDlg::OnBnClickedOk()
{
	if (UpdateDataFromDialog())
		CMyDialogEx::OnOK();
}
CString CWorkPeriodDlg::GetDaysText()
{
	int nDays = 0;
	CString sDays(_T("("));
	for (int iDay = 0; iDay < 7; iDay++)
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
	swprintf_s(zBuf, sizeof(zBuf)/sizeof(wchar_t), L"%d working days per week ", nDays);
	CString s(zBuf);
	s += sDays;
	s += L"\r\n";
	return s;
}
void CWorkPeriodDlg::OnBnClickedCheckSunday()
{
	UpdateText();
}
void CWorkPeriodDlg::OnBnClickedCheckMonday()
{
	UpdateText();
}
void CWorkPeriodDlg::OnBnClickedCheckTuesday()
{
	UpdateText();
}
void CWorkPeriodDlg::OnBnClickedCheckWednesday()
{
	UpdateText();
}
void CWorkPeriodDlg::OnBnClickedCheckThursday()
{
	UpdateText();
}
void CWorkPeriodDlg::OnBnClickedCheckFriday()
{
	UpdateText();
}
void CWorkPeriodDlg::OnBnClickedCheckSaturday()
{
	UpdateText();
}
void CWorkPeriodDlg::OnBnClickedOk3()
{
	if (!UpdateDataFromDialog())
		return;

	if (!gWorkPeriod.IsValid())
	{
		CUtils::MessBox(L"Work period not fully defined", L"Notice");
		return;
	}

	if (gUsedVacations.IsEmpty())
		gUsedVacations.mbAdd14DaysUnpaidVacation4Severance = gConfig.mb14DaysUnpaidVacation4SeveranceDefault;

	CVacationsDlg dlg;
	dlg.DoModal();

	UpdateText();
}
void CWorkPeriodDlg::OnBnClickedButtonFamilyPart()
{
	UpdateDataFromDialog();
	CFamilyPartDlg dlg;
	dlg.DoModal();

	UpdateText();
}
void CWorkPeriodDlg::OnBnClickedCheckNoNotice()
{
	gWorkPeriod.mbSkipNotice = IsChecked(IDC_CHECK_NO_NOTICE);
	UpdateText();
}
void CWorkPeriodDlg::OnBnClickedButtonEditWage()
{
	UpdateDataFromDialog();
	if (!gWorkPeriod.IsValid(false /*!bMustDefineDays*/))
	{
		CUtils::MessBox(L"Please define work period", L"Notice");
		return;
	}

	CWagePeriodsDlg dlg;
	dlg.DoModal();
	SetWageGui();
	UpdateText();
}
void CWorkPeriodDlg::OnBnClickedRadioDiffWages()
{
	OnBnClickedButtonEditWage();
}
void CWorkPeriodDlg::OnBnClickedButtonSetWage()
{
	if (SetWageForWholePeriod())
		UpdateText();
	else
		CUtils::MessBox(L"Missing parameters for wage definition", L"Input error");
}
bool CWorkPeriodDlg::SetWageForWholePeriod()
{
	if (mRadioMinWage.GetCheck() == BST_CHECKED)
	{
		gWage.SetMinWage();
		return true;
	}
	double wage = 0;
	if (mRadioMonthly.GetCheck() == BST_CHECKED)
	{
		if (GetParameter(IDC_EDIT_MONTH_SALARY, wage, 0, 1000000))
		{
			if (wage > 0)
			{
				gWage.SetMonthlyWage(wage);
				return true;
			}
			else
			{
				CUtils::MessBox(L"Monthly wage is not defined", L"Input Error");
				return false;
			}
		}
		return false;
	}
	if (mRadioHourly.GetCheck() == BST_CHECKED)
	{
		double nHours = 0;
		if (GetParameter(IDC_EDIT_HOUR_SALARY, wage, 0, 10000))
		{
			if (wage > 0)
			{
				if (GetParameter(IDC_EDIT_HOURS_PER_MONTH, nHours, 0, 10000))
				{
					if (nHours > 0)
					{
						gWage.SetHourlyWage(wage, nHours);
						return true;
					}
					else
					{
						CUtils::MessBox(L"Number of hours is not defined", L"Input Error");
						return false;
					}
				}
				else
				{
					CUtils::MessBox(L"Hourly wage is not defined", L"Input Error");
					return false;
				}
			}
		}
		return false;
	}
	return true;
}

void CWorkPeriodDlg::OnBnClickedCheckMonthlyBonus()
{
	bool bMonthly = IsChecked(IDC_CHECK_MONTHLY_BONUS);
	SetVisible(IDC_STATIC_MONTHLY_BONUS_TEXT, bMonthly);
	SetVisible(IDC_EDIT_MONTHLY_BONUS, bMonthly);
}
