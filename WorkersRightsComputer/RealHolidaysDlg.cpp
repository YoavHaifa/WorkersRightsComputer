// RealHolidaysDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorkersRightsComputer.h"
#include "RealHolidaysDlg.h"
#include "afxdialogex.h"
#include "Utils.h"
#include "FilesList.h"


// CRealHolidaysDlg dialog

IMPLEMENT_DYNAMIC(CRealHolidaysDlg, CDialogEx)

CRealHolidaysDlg::CRealHolidaysDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_REAL_HOLIDAYS, pParent)
{

}

CRealHolidaysDlg::~CRealHolidaysDlg()
{
}

void CRealHolidaysDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_HOLIDAYS, mComboHolidays);
}

BOOL CRealHolidaysDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitHolidaysCombo();

}

BEGIN_MESSAGE_MAP(CRealHolidaysDlg, CDialogEx)
END_MESSAGE_MAP()


// CRealHolidaysDlg message handlers
void CRealHolidaysDlg::InitHolidaysCombo()
{
	CString msDir(CUtils::GetBaseDir() + L"input\\holidays");
	CFilesList list;
	CUtils::ListFilesInDir(msDir, L"txt", list);
	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CString* psName = list.GetNext(pos);
		CFileName fName(*psName);
		CString sPrivate = fName.PrivateWithoutType();
		if (sPrivate.Right(8) == "Holidays")
			sPrivate = sPrivate.Left(sPrivate.GetLength() - 8);
		mComboHolidays.AddString(sPrivate);
	}
}
