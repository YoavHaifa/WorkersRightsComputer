// COnOpenDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "pch.h"
#include "WorkersRightsComputer.h"
#include "OnOpenDlg.h"
#include "afxdialogex.h"
#include "Config.h"
#include "MyFolderDialog.h"
#include "Utils.h"


// COnOpenDlg dialog

IMPLEMENT_DYNAMIC(COnOpenDlg, CDialogEx)

COnOpenDlg::COnOpenDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG_ON_OPEN, pParent)
	, mbDefineDirCanceled(false)
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

	if (!VerifySaveDir())
		exit(1);

	SetText(IDC_EDIT_SAVE_DIR, gConfig.msSaveRoot);
	SetText(IDC_EDIT_CONTACT_TEL, gConfig.msContactPhone);
	SetText(IDC_EDIT_CONTACT_FAX, gConfig.msContactFax);
	SetText(IDC_EDIT_CONTACT_EMAIL, gConfig.msContactEmail);
	SetText(IDC_EDIT_OPERATOR_ENGLISH, gConfig.msFilledBy);
	SetText(IDC_EDIT_OPERATOR_HEBREW, gConfig.msFilledByHebrew);

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
		gConfig.msSaveRoot = dlg.msFolderName;
	}
	else
		mbDefineDirCanceled = true;
}

void COnOpenDlg::UpdateFromGUI()
{
	GetParameter(IDC_EDIT_SAVE_DIR, gConfig.msSaveRoot);
	GetParameter(IDC_EDIT_CONTACT_TEL, gConfig.msContactPhone);
	GetParameter(IDC_EDIT_CONTACT_FAX, gConfig.msContactFax);
	GetParameter(IDC_EDIT_CONTACT_EMAIL, gConfig.msContactEmail);
	GetParameter(IDC_EDIT_OPERATOR_ENGLISH, gConfig.msFilledBy);
	GetParameter(IDC_EDIT_OPERATOR_HEBREW, gConfig.msFilledByHebrew);
}

bool COnOpenDlg::VerifySaveDir()
{
	int nFailed = -1;
	while (gConfig.msSaveRoot.IsEmpty() || !CUtils::VerifyDirectory(gConfig.msSaveRoot))
	{
		OnBnClickedButtonDefineDir();
		if (mbDefineDirCanceled)
			exit(1);

		nFailed++;
		if (nFailed > 3)
			exit(1);
	}
	return true;
}
