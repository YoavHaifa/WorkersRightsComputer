// WagePeriods.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "WagePeriodsDlg.h"
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
}


BEGIN_MESSAGE_MAP(CWagePeriodsDlg, CDialogEx)
END_MESSAGE_MAP()


// CWagePeriods message handlers



BOOL CWagePeriodsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	UpdateState();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWagePeriodsDlg::UpdateState()
{
	CString sState("Wage Periods\r\n");

	mShowPeriods.SetWindowText(sState);
}
