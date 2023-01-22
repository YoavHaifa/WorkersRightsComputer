// LetterSavedDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "pch.h"
#include "WorkersRightsComputer.h"
#include "LetterSavedDlg.h"
#include "afxdialogex.h"


// CLetterSavedDlg dialog

IMPLEMENT_DYNAMIC(CLetterSavedDlg, CDialogEx)

CLetterSavedDlg::CLetterSavedDlg(CWnd* pParent /*=nullptr*/)
	: CMyDialogEx(IDD_DIALOG2, pParent)
{

}

CLetterSavedDlg::~CLetterSavedDlg()
{
}

void CLetterSavedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CLetterSavedDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetText(IDC_STATIC_FILE_NAME, msFileName);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CLetterSavedDlg, CDialogEx)
END_MESSAGE_MAP()


// CLetterSavedDlg message handlers


void CLetterSavedDlg::SetFileName(CString& sfName)
{
	msFileName = sfName;
}
