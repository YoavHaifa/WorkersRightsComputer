// COnOpenDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "pch.h"
#include "WorkersRightsComputer.h"
#include "OnOpenDlg.h"
#include "afxdialogex.h"
#include "Config.h"
#include "MyFolderDialog.h"


// COnOpenDlg dialog

IMPLEMENT_DYNAMIC(COnOpenDlg, CDialogEx)

COnOpenDlg::COnOpenDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_ON_OPEN, pParent)
{

}

COnOpenDlg::~COnOpenDlg()
{
}

void COnOpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL COnOpenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetText(IDC_EDIT_SAVE_DIR, gConfig.msSaveRoot);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
void COnOpenDlg::OnOK()
{
	UpdateFromGUI();
	gConfig.SaveState();
	CDialogEx::OnOK();
}
void COnOpenDlg::OnCancel()
{
	exit(1);
}

BEGIN_MESSAGE_MAP(COnOpenDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DEFINE_DIR, &COnOpenDlg::OnBnClickedButtonDefineDir)
END_MESSAGE_MAP()

// COnOpenDlg message handlers

void COnOpenDlg::OnBnClickedButtonDefineDir()
{
	CMyFolderDialog dlg("Choose Directory to Save Cases");
	if (dlg.DoModal())
	{
		SetText(IDC_EDIT_SAVE_DIR, dlg.msFolderName);
	}
}

void COnOpenDlg::UpdateFromGUI()
{
	GetParameter(IDC_EDIT_SAVE_DIR, gConfig.msSaveRoot);
}
