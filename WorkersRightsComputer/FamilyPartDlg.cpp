// FamilyPartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "FamilyPartDlg.h"
#include "FamilyPart.h"
#include "CompanyPartPeriod.h"
#include "WorkPeriod.h"
#include "afxdialogex.h"


// CFamilyPartDlg dialog

IMPLEMENT_DYNAMIC(CFamilyPartDlg, CDialogEx)

CFamilyPartDlg::CFamilyPartDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_FAMILY_PART, pParent)
{

}

CFamilyPartDlg::~CFamilyPartDlg()
{
}

void CFamilyPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, mStartPeriodGUI);
}


BEGIN_MESSAGE_MAP(CFamilyPartDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PERIOD, &CFamilyPartDlg::OnBnClickedButtonAddPeriod)
	ON_BN_CLICKED(IDC_BUTTON_FAMILY_PART_CLEAR, &CFamilyPartDlg::OnBnClickedButtonFamilyPartClear)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PERIOD_PERCENT, &CFamilyPartDlg::OnBnClickedButtonAddPeriodPercent)
	ON_BN_CLICKED(IDC_BUTTON_FAMILY_PART_CLEAR_LAST, &CFamilyPartDlg::OnBnClickedButtonFamilyPartClearLast)
	ON_BN_CLICKED(IDC_RADIO_COMPANY_MINIMUM, &CFamilyPartDlg::OnBnClickedRadioCompanyMinimum)
	ON_BN_CLICKED(IDC_RADIO_COMPANY_HOURLY, &CFamilyPartDlg::OnBnClickedRadioCompanyHourly)
END_MESSAGE_MAP()

BOOL CFamilyPartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ClearForEdit();
	UpdateText();
	return TRUE;  // return TRUE  unless you set the focus to a contro
}

// CFamilyPartDlg message handlers

void CFamilyPartDlg::UpdateText()
{
	if (!gFamilyPart.mbDefined)
	{
		if (gWorkPeriod.mFirst.mbInitialized)
		{
			mStartPeriodGUI.SetTime(&gWorkPeriod.mFirst.mTime);
		}
		ClearForEdit();
	}

	CString s = gFamilyPart.GetFullText();
	SetText(IDC_EDIT_FAMILY_PART_STATUS, s);

	SetCheck(IDC_CHECK_FAMILY_PART, gFamilyPart.mbAskOnlyForFamilyPart);
}
bool CFamilyPartDlg::CheckInputValue(int id, double& oValue, double minVal, double maxVal, const wchar_t* zWhat)
{
	oValue = 0;
	CString s(GetText(id));
	if (!isdigit(s[0]))
	{
		CString s(L"Please define ");
		s += zWhat;
		MessageBox(s, L"Input Error");
		return false;
	}
	double value = _wtof(s);
	if (value < minVal || value > maxVal)
	{
		CString s(L"Illegal value for ");
		s += zWhat;
		MessageBox(s, L"Input Error");
		return false;
	}
	oValue = value;
	return true;
}
void CFamilyPartDlg::AddPeriod(bool bByHour)
{
	mPC = 0;
	mHoursPerWeek = 0;

	if (bByHour)
	{
		if (!CheckInputValue(IDC_EDIT_HOURS_BY_COMPANY, mHoursPerWeek, 0, 43, L"number of company hours per week"))
			return;
	}
	else //  By PC
	{
		if (!CheckInputValue(IDC_EDIT_PERCENT_BY_COMPANY, mPC, 0, 100, L"company percent"))
			return;
	}

	DWORD dwResult = mStartPeriodGUI.GetTime(mStartTime);
	if (dwResult != GDT_VALID)
	{
		MessageBox(L"Illegal date for period start", L"Input Error");
		return;
	}
	mbMinimum = IsChecked(IDC_RADIO_COMPANY_MINIMUM);
	if (mbMinimum)
		mHourlyWage = 0;
	else
	{
		if (!CheckInputValue(IDC_EDIT_COMPANY_HOURLY_RATE, mHourlyWage, 1, 1000, L"company wage per hour"))
			return;
	}

	CCompanyPartPeriod* pNewPeriod = new CCompanyPartPeriod(*this);
	gFamilyPart.AddPeriod(pNewPeriod);
	UpdateText();
	ClearForEdit();
}
void CFamilyPartDlg::OnBnClickedButtonAddPeriod()
{
	AddPeriod(true);
}
void CFamilyPartDlg::OnBnClickedButtonAddPeriodPercent()
{
	AddPeriod(false);
}
void CFamilyPartDlg::OnBnClickedButtonFamilyPartClear()
{
	gFamilyPart.Clear();
	UpdateText();
}
void CFamilyPartDlg::OnBnClickedButtonFamilyPartClearLast()
{
	gFamilyPart.ClearLast();
	UpdateText();
}
void CFamilyPartDlg::ClearForEdit()
{
	SetText(IDC_EDIT_HOURS_BY_COMPANY, L"");
	SetText(IDC_EDIT_PERCENT_BY_COMPANY, L"");
	SetText(IDC_EDIT_COMPANY_HOURLY_RATE, L"");
	SetCheck(IDC_RADIO_COMPANY_MINIMUM, true); 
	SetCheck(IDC_RADIO_COMPANY_HOURLY, false);
	SetInvisible(IDC_EDIT_COMPANY_HOURLY_RATE);
}
void CFamilyPartDlg::OnBnClickedRadioCompanyMinimum()
{
	SetText(IDC_EDIT_COMPANY_HOURLY_RATE, L"");
	SetInvisible(IDC_EDIT_COMPANY_HOURLY_RATE);
}
void CFamilyPartDlg::OnBnClickedRadioCompanyHourly()
{
	SetVisible(IDC_EDIT_COMPANY_HOURLY_RATE);
}
