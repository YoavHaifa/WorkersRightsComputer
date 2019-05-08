// FamilyPartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "FamilyPartDlg.h"
#include "FamilyPart.h"
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
	DDX_Control(pDX, IDC_DATETIMEPICKER1, mStartPeriod);
}


BEGIN_MESSAGE_MAP(CFamilyPartDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PERIOD, &CFamilyPartDlg::OnBnClickedButtonAddPeriod)
	ON_BN_CLICKED(IDC_BUTTON_FAMILY_PART_CLEAR, &CFamilyPartDlg::OnBnClickedButtonFamilyPartClear)
END_MESSAGE_MAP()

BOOL CFamilyPartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	SetText(IDC_EDIT_HOURS_BY_COMPANY, L"-");


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
			mStartPeriod.SetTime(&gWorkPeriod.mFirst.mTime);
		}
		SetText(IDC_EDIT_HOURS_BY_COMPANY, L"-");
	}

	CString s = gFamilyPart.GetFullText();
	SetText(IDC_EDIT_FAMILY_PART_STATUS, s);

	SetCheck(IDC_CHECK_FAMILY_PART, gFamilyPart.mbAskOnlyForFamilyPart);
}
void CFamilyPartDlg::OnBnClickedButtonAddPeriod()
{
	CString s(GetText(IDC_EDIT_HOURS_BY_COMPANY));
	if (!isdigit(s[0]))
	{
		MessageBox(L"Please define number of hours per week", L"Input Error");
		return;
	}
	double hoursPerWeek = _wtof(s);
	if (hoursPerWeek < 0 || hoursPerWeek > 43)
	{
		MessageBox(L"Illegal number of hours per week", L"Input Error");
		return;
	}

	CTime timeTime;
	DWORD dwResult = mStartPeriod.GetTime(timeTime);
	if (dwResult == GDT_VALID)
	{
		gFamilyPart.AddPeriod(timeTime, hoursPerWeek);
	}
	else
	{
		MessageBox(L"Illegal date for period start", L"Input Error");
		return;
	}
	UpdateText();
	SetText(IDC_EDIT_HOURS_BY_COMPANY, L"-");
}
void CFamilyPartDlg::OnBnClickedButtonFamilyPartClear()
{
	gFamilyPart.Clear();
	UpdateText();
}
