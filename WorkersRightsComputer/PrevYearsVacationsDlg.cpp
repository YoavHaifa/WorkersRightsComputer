// PrevYearsVacationsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "PrevYearsVacationsDlg.h"
#include "afxdialogex.h"
#include "VacationDaysDue.h"


// CPrevYearsVacationsDlg dialog

IMPLEMENT_DYNAMIC(CPrevYearsVacationsDlg, CDialogEx)

CPrevYearsVacationsDlg::CPrevYearsVacationsDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_PREV_YEARS_VACATIONS, pParent)
	, mbInitializing(true)
{

}

CPrevYearsVacationsDlg::~CPrevYearsVacationsDlg()
{
}

void CPrevYearsVacationsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPrevYearsVacationsDlg, CDialogEx)
END_MESSAGE_MAP()


BOOL CPrevYearsVacationsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	gVacationDaysDue.InitDialog(this);
	mbInitializing = false;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// CPrevYearsVacationsDlg message handlers
