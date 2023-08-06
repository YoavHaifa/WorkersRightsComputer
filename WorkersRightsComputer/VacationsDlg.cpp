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
	, mnCurrentWeeks(30)
	, mbStartSet(false)
	, mbEndSet(false)
	, mbMaternity(false)
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
	DDX_Control(pDX, IDC_CHECK_MATERNITY, mIsMaternityLeave);
	DDX_Control(pDX, IDC_EDIT_MATERNITY_N_PAID_WEEKS, mnPaidMLWeeks);
	DDX_Control(pDX, IDC_CHECK_MATERNITY_PENSION, mDeservesPension);
}


BEGIN_MESSAGE_MAP(CVacationsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_VAC_ADD, &CVacationsDlg::OnBnClickedButtonVacAdd)
	ON_BN_CLICKED(IDC_BUTTON_VAC_CLEAR_LAST, &CVacationsDlg::OnBnClickedButtonVacClearLast)
	ON_BN_CLICKED(IDC_BUTTON_VAC_CLEAR, &CVacationsDlg::OnBnClickedButtonVacClear)
	ON_BN_CLICKED(IDC_CHECK_14DAYS_VAC4SEVRANCE, &CVacationsDlg::OnBnClickedCheck14daysVac4sevrance)
	ON_BN_CLICKED(IDC_CHECK_MATERNITY, &CVacationsDlg::OnBnClickedCheckMaternity)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_VAC_START, &CVacationsDlg::OnDtnDatetimechangeDatetimepickerVacStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_VAC_END, &CVacationsDlg::OnDtnDatetimechangeDatetimepickerVacEnd)
END_MESSAGE_MAP()

BOOL CVacationsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (gUsedVacations.mbAdd14DaysUnpaidVacation4Severance)
		SetCheck(IDC_CHECK_14DAYS_VAC4SEVRANCE, true);

	UpdateText();
	UpdateDisplayedLength();
	SetMaternityLeave();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVacationsDlg::SetMaternityLeave()
{
	SetCheck(IDC_CHECK_MATERNITY, mbMaternity);
	SetVisible(IDC_STATIC_MATERNITY_TEXT, mbMaternity);
	SetVisible(IDC_EDIT_MATERNITY_N_PAID_WEEKS, mbMaternity);
	SetVisible(IDC_CHECK_MATERNITY_PENSION, mbMaternity);
	if (mbMaternity)
	{
		SetParameter(IDC_EDIT_MATERNITY_N_PAID_WEEKS, 15);
		SetCheck(IDC_CHECK_MATERNITY_PENSION, false);
	}
	UpdateDisplayedLength();
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

	//CVacationUsed *pVac = new CVacationUsed(mStart, mEnd);
	bool bMaternity = IsChecked(IDC_CHECK_MATERNITY);
	int nPaidWeeks = 0;
	bool bMaternityPension = false;
	if (bMaternity)
	{
		GetParameter(IDC_EDIT_MATERNITY_N_PAID_WEEKS, nPaidWeeks, 0, (int)(mnCurrentWeeks+0.99));
		bMaternityPension = IsChecked(IDC_CHECK_MATERNITY_PENSION);
	}
	gUsedVacations.AddVacation(mStart, mEnd, bMaternity, nPaidWeeks, bMaternityPension);
	UpdateText();
	mbStartSet = false;
	mbEndSet = false;
	mbMaternity = false;
	SetMaternityLeave();
}
void CVacationsDlg::UpdateDisplayedLength()
{
	if (mbStartSet && mbEndSet)
	{
		static const int BUF_LEN = 256;
		wchar_t zBuf[BUF_LEN];
		if (mbMaternity)
		{
			double nWeeks = mStart.GetNWeeksUntil(mEnd);
			swprintf_s(zBuf, BUF_LEN, L"%.2f weeks", nWeeks);
		}
		else
		{
			int nDays = mStart.GetNDaysUntil(mEnd);
			swprintf_s(zBuf, BUF_LEN, L"%d days", nDays);
		}
		SetText(IDC_STATIC_N_VACATION_WEEKS, zBuf);
	}
	else
	{
		if (!mbStartSet)
			SetInvisible(IDC_DATETIMEPICKER_VAC_END);
		Clear(IDC_STATIC_N_VACATION_WEEKS);
	}
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
void CVacationsDlg::OnBnClickedCheckMaternity()
{
	mbMaternity = IsChecked(IDC_CHECK_MATERNITY);
	SetMaternityLeave();
}
void CVacationsDlg::OnDtnDatetimechangeDatetimepickerVacStart(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	CTime time;
	mEditFirstDay.GetTime(time);
	mStart.SetDate(time);
	if (!mbStartSet)
	{
		CTime time1;
		mEditFirstDay.GetTime(time1);
		mEditLastDay.SetTime(&time1);
		SetVisible(IDC_DATETIMEPICKER_VAC_END);
	}
	mbStartSet = true;
	*pResult = 0;
	UpdateDisplayedLength();
}
void CVacationsDlg::OnDtnDatetimechangeDatetimepickerVacEnd(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	CTime time;
	mEditLastDay.GetTime(time);
	mEnd.SetDate(time);
	mbEndSet = true;
	*pResult = 0;
	UpdateDisplayedLength();
}
