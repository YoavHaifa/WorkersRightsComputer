// PrevYearsHolidaysDlg.cpp : implementation file
//

//#include "pch.h"
#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "PrevYearsHolidaysDlg.h"
#include "afxdialogex.h"
#include "HolidaysDue.h"


// CPrevYearsHolidaysDlg dialog

IMPLEMENT_DYNAMIC(CPrevYearsHolidaysDlg, CDialogEx)

CPrevYearsHolidaysDlg::CPrevYearsHolidaysDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_PREV_HOLIDAYS, pParent)
	, mbInitializing(true)
{

}

CPrevYearsHolidaysDlg::~CPrevYearsHolidaysDlg()
{
}

void CPrevYearsHolidaysDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPrevYearsHolidaysDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_WORK, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_PAID, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_FROM, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_DUE, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy)

	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_WORK1, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy1)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_PAID1, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy1)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_FROM1, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy1)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_DUE1, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy1)

	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_WORK2, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy2)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_PAID2, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy2)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_FROM2, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy2)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_DUE2, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy2)

	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_WORK3, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy3)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_PAID3, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy3)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_FROM3, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy3)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_DUE3, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy3)

	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_WORK4, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy4)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_PAID4, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy4)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_FROM4, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy4)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_DUE4, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy4)

	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_WORK5, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy5)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_PAID5, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy5)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_FROM5, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy5)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_DUE5, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy5)

	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_WORK6, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy6)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_PAID6, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy6)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_FROM6, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy6)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_DUE6, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy6)

	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_WORK7, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy7)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_PAID7, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy7)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_FROM7, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy7)
	ON_EN_CHANGE(IDC_EDIT_HOLIDAYS_PREVY_DUE7, &CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy7)
END_MESSAGE_MAP()

BOOL CPrevYearsHolidaysDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	gHolidaysDue.InitDialog(this);
	mbInitializing = false;
	return TRUE;  // return TRUE  unless you set the focus to a control
}
// CPrevYearsHolidaysDlg message handlers

void CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CMyDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	static int count = 0;
	count++;
	if (mbInitializing)
		return;

	gHolidaysDue.OnGuiChange(0, this);
}
void CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy1()
{
	if (mbInitializing)
		return;
	gHolidaysDue.OnGuiChange(1, this);
}
void CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy2()
{
	if (mbInitializing)
		return;
	gHolidaysDue.OnGuiChange(2, this);
}
void CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy3()
{
	if (mbInitializing)
		return;
	gHolidaysDue.OnGuiChange(3, this);
}
void CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy4()
{
	if (mbInitializing)
		return;
	gHolidaysDue.OnGuiChange(4, this);
}
void CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy5()
{
	if (mbInitializing)
		return;
	gHolidaysDue.OnGuiChange(5, this);
}
void CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy6()
{
	if (mbInitializing)
		return;
	gHolidaysDue.OnGuiChange(6, this);
}
void CPrevYearsHolidaysDlg::OnEnChangeEditHolidaysPrevy7()
{
	if (mbInitializing)
		return;
	gHolidaysDue.OnGuiChange(7, this);
}
