// FamilyPartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "FamilyPartDlg.h"
#include "FamilyPart.h"
#include "CompanyPartPeriod.h"
#include "WorkPeriod.h"
#include "afxdialogex.h"
#include "MyAskDlg.h"


// CFamilyPartDlg dialog

IMPLEMENT_DYNAMIC(CFamilyPartDlg, CDialogEx)

CFamilyPartDlg::CFamilyPartDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_FAMILY_PART, pParent)
	, mbInitialized(false)
	, mbAvoidInfiniteLoop(false)
{
	mSavedDefinitions.Copy(gFamilyPart);
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
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CFamilyPartDlg::OnDtnDatetimechangeDatetimepicker1)
	ON_EN_CHANGE(IDC_EDIT_HOURS_BY_COMPANY, &CFamilyPartDlg::OnEnChangeEditHoursByCompany)
	ON_EN_CHANGE(IDC_EDIT_PERCENT_BY_COMPANY, &CFamilyPartDlg::OnEnChangeEditPercentByCompany)
	ON_EN_CHANGE(IDC_EDIT_COMPANY_HOURLY_RATE, &CFamilyPartDlg::OnEnChangeEditCompanyHourlyRate)
END_MESSAGE_MAP()

BOOL CFamilyPartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ClearForEdit();
	UpdateText();
	mbUnsavedChanges = false;
	mbInitialized = true;
	return TRUE;  // return TRUE  unless you set the focus to a contro
}
void CFamilyPartDlg::OnOK()
{
	if (mbUnsavedChanges)
	{
		CMyAskDlg dlg(L"Verify OK", L"Last editing was not entered. Exit dialog anyway?");
		if (!dlg.Ask())
			return;
	}
	CMyDialogEx::OnOK();
}
void CFamilyPartDlg::OnCancel()
{
	gFamilyPart.Copy(mSavedDefinitions);
	CMyDialogEx::OnCancel();
}
// CFamilyPartDlg message handlers

void CFamilyPartDlg::UpdateText()
{
	if (!gFamilyPart.mbDefined)
		ClearForEdit();

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
	mbUnsavedChanges = false;
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
	ClearForEdit();
}
void CFamilyPartDlg::OnBnClickedButtonFamilyPartClearLast()
{
	gFamilyPart.ClearLast();
	UpdateText();
	ClearForEdit();
}
void CFamilyPartDlg::ClearForEdit()
{
	SetText(IDC_EDIT_HOURS_BY_COMPANY, L"");
	SetText(IDC_EDIT_PERCENT_BY_COMPANY, L"");
	SetText(IDC_EDIT_COMPANY_HOURLY_RATE, L"");
	SetCheck(IDC_RADIO_COMPANY_MINIMUM, true); 
	SetCheck(IDC_RADIO_COMPANY_HOURLY, false);
	SetInvisible(IDC_EDIT_COMPANY_HOURLY_RATE);

	SetText(IDC_STATIC_FP_EDITING, L"");

	if (!gFamilyPart.mbDefined)
	{
		if (gWorkPeriod.mFirst.mbInitialized)
			mStartPeriodGUI.SetTime(&gWorkPeriod.mFirst.mTime);
	}
	else
	{
		CCompanyPartPeriod* pLastPeriod = gFamilyPart.GetLastPeriod();
		if (pLastPeriod)
		{
			mStartPeriodGUI.SetTime(&pLastPeriod->GetStartTime().mTime);
			if (pLastPeriod->mHoursPerWeek)
				SetParameter(IDC_EDIT_HOURS_BY_COMPANY, pLastPeriod->mHoursPerWeek);
			else if (pLastPeriod->mPC)
				SetParameter(IDC_EDIT_PERCENT_BY_COMPANY, pLastPeriod->mPC);

			if (!pLastPeriod->mbCompanyPaysMinimumWage)
			{
				SetParameter(IDC_EDIT_COMPANY_HOURLY_RATE, pLastPeriod->mHourlyWage);
				SetCheck(IDC_RADIO_COMPANY_MINIMUM, false);
				SetCheck(IDC_RADIO_COMPANY_HOURLY, true);

				SetVisible(IDC_EDIT_COMPANY_HOURLY_RATE);
			}
		}
	}
}
void CFamilyPartDlg::OnBnClickedRadioCompanyMinimum()
{
	SetText(IDC_EDIT_COMPANY_HOURLY_RATE, L"");
	SetInvisible(IDC_EDIT_COMPANY_HOURLY_RATE);
	OnEdit();
}
void CFamilyPartDlg::OnBnClickedRadioCompanyHourly()
{
	SetVisible(IDC_EDIT_COMPANY_HOURLY_RATE);
	OnEdit();
}
void CFamilyPartDlg::OnDtnDatetimechangeDatetimepicker1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	OnEdit();
	*pResult = 0;
}
void CFamilyPartDlg::OnEnChangeEditHoursByCompany()
{
	if (mbAvoidInfiniteLoop)
		return;
	mbAvoidInfiniteLoop = true;
	SetText(IDC_EDIT_PERCENT_BY_COMPANY, L"");
	mbAvoidInfiniteLoop = false;
	OnEdit();
}
void CFamilyPartDlg::OnEnChangeEditPercentByCompany()
{
	if (mbAvoidInfiniteLoop)
		return;
	mbAvoidInfiniteLoop = true;
	SetText(IDC_EDIT_HOURS_BY_COMPANY, L"");
	mbAvoidInfiniteLoop = false;
	OnEdit();
}
void CFamilyPartDlg::OnEnChangeEditCompanyHourlyRate()
{
	OnEdit();
}
void CFamilyPartDlg::OnEdit()
{
	if (!mbInitialized)
		return;

	CTime startTime;
	mStartPeriodGUI.GetTime(startTime);

	mbUnsavedChanges = false;

	CCompanyPartPeriod* pLastPeriod = gFamilyPart.GetLastPeriod();
	if (pLastPeriod)
	{
		if (startTime != pLastPeriod->GetStartTime().mTime)
			mbUnsavedChanges = true;
		if (!VerifyParameter(IDC_EDIT_HOURS_BY_COMPANY, pLastPeriod->mHoursPerWeek))
			mbUnsavedChanges = true;
		if (!VerifyParameter(IDC_EDIT_PERCENT_BY_COMPANY, pLastPeriod->mPC))
			mbUnsavedChanges = true;
		if (pLastPeriod->mbCompanyPaysMinimumWage)
		{
			if (!IsChecked(IDC_RADIO_COMPANY_MINIMUM))
				mbUnsavedChanges = true;
		}
		else
		{
			if (!IsChecked(IDC_RADIO_COMPANY_HOURLY))
				mbUnsavedChanges = true;

			if (!VerifyParameter(IDC_EDIT_COMPANY_HOURLY_RATE, pLastPeriod->mHourlyWage))
				mbUnsavedChanges = true;
		}
	}
	else
	{
		if (startTime != gWorkPeriod.mFirst.mTime)
			mbUnsavedChanges = true;
		if (!VerifyText(IDC_EDIT_HOURS_BY_COMPANY, L""))
			mbUnsavedChanges = true;
		if (!VerifyText(IDC_EDIT_PERCENT_BY_COMPANY, L""))
			mbUnsavedChanges = true;
		if (!VerifyText(IDC_EDIT_COMPANY_HOURLY_RATE, L""))
			mbUnsavedChanges = true;
		if (!IsChecked(IDC_RADIO_COMPANY_MINIMUM))
			mbUnsavedChanges = true;
	}

	if (mbUnsavedChanges)
		SetText(IDC_STATIC_FP_EDITING, L"*** Editing ***");
	else
		SetText(IDC_STATIC_FP_EDITING, L"");
}